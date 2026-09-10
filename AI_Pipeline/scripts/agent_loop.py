#!/usr/bin/env python3
"""agent_loop.py — orquestrador: o "agente" que segue o fluxo automático.

Máquina de estados (manifest work/manifests/<slug>.json):
  plan → draft → textured → cleaned → rigged → animated → approved
  (com rodadas de crítica: até max_fix_rounds, depois quarantine)

Agentes executados: A1 gen_asset.py · A2 paint_asset.py · A3 cleanup_mesh.py
· A4 rigify_rig.py · A5 mocap_capture/retarget · A6 qc.py + import_to_ue.py
· A0 critic_client.py (Qwen3-VL via LM Studio) nos gates de crítica.

Uso:
  agent_loop.py --slug prop_weapon_sword_v001 --prompt "..." [--ref f.png]
      [--auto]                      # ignora o gate humano em 'approved'
      [--iterations N]              # máx. de ciclos de feedback
      [--resume]                    # retoma do estado do manifest
"""
import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = json.loads((ROOT / "config" / "pipeline.json").read_text())
BLENDER = CONFIG["blender_app"]
SCRIPT = ROOT / "scripts"

STAGE = [
    ("draft",    lambda a: subprocess.run([sys.executable, str(SCRIPT / "gen_asset.py"),
                                           "--slug", a.slug, "--prompt", a.prompt,
                                           *(["--ref", str(a.ref)] if a.ref else [])])),
    ("textured", lambda a: subprocess.run([sys.executable, str(SCRIPT / "paint_asset.py"),
                                           "--slug", a.slug])),
    ("cleaned",  lambda a: subprocess.run([BLENDER, "-b", "--python",
                                           str(SCRIPT / "cleanup_mesh.py"), "--",
                                           "--slug", a.slug, "--render"], timeout=1500)),
    ("rigged",   lambda a: subprocess.run([BLENDER, "-b", "--python",
                                           str(SCRIPT / "rigify_rig.py"), "--",
                                           "--slug", a.slug], timeout=900)),
    ("animated", lambda a: subprocess.run([BLENDER, "-b", "--python",
                                           str(SCRIPT / "mocap_retarget.py"), "--",
                                           "--slug", a.slug, "--action", a.action], timeout=900)),
]


class Args:
    def __init__(self, slug, prompt, ref, action="idle"):
        self.slug, self.prompt, self.ref, self.action = slug, prompt, ref, action


def manifest(slug: str) -> dict:
    p = ROOT / "work" / "manifests" / f"{slug}.json"
    if p.exists():
        return json.loads(p.read_text())
    return {"slug": slug, "status": "plan", "stages": {}, "rounds": 0,
            "notes": [], "critiques": []}


def save(slug: str, m: dict) -> None:
    p = ROOT / "work" / "manifests" / f"{slug}.json"
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps(m, indent=2, ensure_ascii=False))


def critic_health() -> bool:
    import urllib.request
    try:
        urllib.request.urlopen(CONFIG["lm_server"]["base_url"] + "/models", timeout=5)
        return True
    except Exception:
        return False


def run_critique(a: Args, m: dict) -> str:
    """A0: crítica com visão (Qwen3-VL) — retorna veredito."""
    work = ROOT / "work" / a.slug
    images = sorted((work / "10_draft" / "turntable").glob(f"{a.slug}__view*.png"))
    if not images:
        print("[loop] sem turntable p/ crítica (crie com --render).")
        return "continuar"
    if not critic_health():
        print("[loop] LM Studio offline — pulando crítica (use --human-gate para decidir).")
        return "continuar"
    r = subprocess.run([sys.executable, str(SCRIPT / "critic_client.py"),
                        "--prompt", str(work / "input" / f"{a.slug}__prompt.md"),
                        "--images", *[str(i) for i in images], "--json"],
                       capture_output=True, text=True, timeout=600)
    try:
        verdict = json.loads(r.stdout)
    except json.JSONDecodeError:
        print(f"[loop] crítica não-parsável: {r.stdout[-300:]}")
        return "continuar"
    m["critiques"].append(verdict)
    print(f"[loop] crítico: veredito={verdict.get('veredito')} faltas={len(verdict.get('faltas', []))}")
    return verdict.get("veredito", "continuar")


def human_gate(slug: str) -> bool:
    ans = input(f"[loop] aprovar {slug} para import no UE? (y/N) ").strip().lower()
    return ans in ("y", "yes")


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--slug", required=True)
    ap.add_argument("--prompt", default="")
    ap.add_argument("--ref", type=Path, default=None)
    ap.add_argument("--action", default="idle")
    ap.add_argument("--auto", action="store_true")
    ap.add_argument("--iterations", type=int, default=CONFIG["loop"]["max_fix_rounds"])
    ap.add_argument("--resume", action="store_true")
    args = ap.parse_args()

    a = Args(args.slug, args.prompt, args.ref, args.action)
    m = manifest(args.slug)
    names = [s for s, _ in STAGE]
    start_idx = names.index(m["status"]) if m.get("status") in names else 0

    for stage_name, run in STAGE[start_idx:]:
        m["status"] = stage_name
        save(args.slug, m)
        print(f"\n[loop] ——— etapa {stage_name} ———")
        if stage_name == "draft" and not a.prompt:
            print("[loop] --prompt vazio em 'plan' — peça o prompt ou use --resume.")
            return 2
        r = run(a)
        if r.returncode != 0 and not (stage_name == "animated" and r.returncode == 1):
            # fail da etapa: tenta regen (rodada de fix) ou quarentena
            m["rounds"] += 1
            if m["rounds"] > CONFIG["loop"]["max_fix_rounds"]:
                m["status"] = "quarantined"
                m["notes"].append(f"falha na etapa {stage_name} após {m['rounds']} rodadas")
                save(args.slug, m)
                print(f"[loop] QUARENTENA — {args.slug} (veja work/{args.slug}/90_quarantine)")
                return 1
            m["notes"].append(f"etapa {stage_name} falhou (rc={r.returncode}), regenerando round {m['rounds']}")
            save(args.slug, m)
            continue
        m["stages"][stage_name] = {"ok": True}
        save(args.slug, m)

        veredito = run_critique(a, m)
        if veredito == "falhar":
            m["status"] = "quarantined"
            save(args.slug, m)
            print("[loop] crítico falhou o asset — quarentena.")
            return 1

    # gate humano + import
    approved = args.auto or human_gate(args.slug)
    m["status"] = "approved"
    save(args.slug, m)
    if approved:
        print(f"[loop] aprovado: {args.slug} -> import_to_ue.py (próximo passo sugerido)")
    else:
        print(f"[loop] {args.slug} marcado como approved no manifest; "
              "rode import_to_ue.py quando quiser importar.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
