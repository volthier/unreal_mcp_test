#!/usr/bin/env python3
"""gen_asset.py — etapa 1: texto/imagem → 3D.

Rotas (config.pipeline.json -> routing):
  hy3d      : Hunyuan3D-2.1-mlx Stage 1 (MLX). Entrada: imagem (ref).
  trellis   : trellis2-mlx (MLX/MPS, 1024³ ou 512³). Entrada: imagem (ref).
  llamamesh : LLaMA-Mesh via LM Studio (texto → código Blender → GLB).
              Entrada: prompt (props/hard-surface).
  flux      : texto → imagem (MLX Flux via ComfyUI ou flux-heed local) — usado
              como pré-passo das rotas hy3d/trellis quando não há ref.

Toda saída respeita a convenção de nomes:
  work/<slug>/10_draft/<slug>.draft.glb (+ <slug>.preview.splat para blockout)

Uso:
  gen_asset.py --slug prop_weapon_sword_v001 --prompt "espada de ferro medieval"
               [--ref refs/espada.png] [--route auto|hy3d|trellis|llamamesh]
               [--tee N]        # roda tripoflux (blockout splat) + rota final
"""
import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = json.loads((ROOT / "config" / "pipeline.json").read_text())


def paths_for(slug: str) -> dict:
    work = ROOT / "work" / slug
    for d in ["input", "10_draft", "20_mesh", "30_rig", "40_anim", "50_game",
              "60_approved", "90_quarantine", "95_archive", "tmp"]:
        (work / d).mkdir(parents=True, exist_ok=True)
    return {
        "slug_dir": work,
        "input": work / "input",
        "draft": work / "10_draft",
        "manifest": ROOT / "work" / "manifests" / f"{slug}.json",
    }


def load_manifest(slug: str) -> dict:
    p = ROOT / "work" / "manifests" / f"{slug}.json"
    if p.exists():
        return json.loads(p.read_text())
    return {"slug": slug, "status": "plan", "stages": {}, "rounds": 0, "notes": []}


def save_manifest(slug: str, m: dict) -> None:
    p = ROOT / "work" / "manifests" / f"{slug}.json"
    p.parent.mkdir(parents=True, exist_ok=True)
    p.write_text(json.dumps(m, indent=2, ensure_ascii=False))


def run_hy3d(slug: str, ref: Path, out: Path, env: str = "hy3d-mlx") -> int:
    """Chama o pipeline MLX do Hunyuan3D Stage 1 dentro do env conda."""
    s1 = CONFIG["hunyuan3d"]["stage1"]
    code = f"""
import sys, json
sys.path.insert(0, {str(ROOT / "engines" / "hunyuan3d-2.1-mlx")!r})
from hy3dshape.hy3dshape.pipeline_mlx import ShapePipeline
cfg = json.load(open({str(ROOT / "config" / "pipeline.json")!r}))
pipe = ShapePipeline.from_pretrained(
    cfg["hunyuan3d"]["weights_dir"] or "dgrauet/hunyuan3d-2.1-mlx")
mesh = pipe({str(ref.resolve())!r},
            num_inference_steps={s1["num_inference_steps"]},
            guidance_scale={s1["guidance_scale"]},
            octree_resolution={s1["octree_resolution"]})
mesh.export({str(out.resolve())!r})
print("EXPORTED", {str(out.resolve())!r})
"""
    script = Path(out).parent / f"{slug}__st1.py"
    script.write_text(code)
    import os
    python = None
    for cand in [os.path.expanduser(f"~/miniforge3/envs/{env}/bin/python"),
                 f"/opt/homebrew/Caskroom/miniforge/base/envs/{env}/bin/python"]:
        if Path(cand).exists():
            python = cand
            break
    if python is None:
        print(f"[gen] env {env} não encontrado (rode scripts/install.sh primeiro).")
        return 2
    r = subprocess.run([python, str(script)], capture_output=True, text=True, timeout=1800)
    print(r.stdout[-1500:], file=sys.stderr)
    if r.returncode != 0:
        print(r.stderr[-2500:], file=sys.stderr)
    return r.returncode


def run_llamamesh(slug: str, prompt: str, out: Path) -> int:
    """LLaMA-Mesh via LM Studio: prompt → Blender Python → executa no Blender."""
    import urllib.request
    payload = json.dumps({
        "model": CONFIG["lm_server"]["llamamesh_model"],
        "messages": [{"role": "user", "content": prompt}],
        "temperature": 0.0, "max_tokens": 4096,
    }).encode()
    req = urllib.request.Request(
        CONFIG["lm_server"]["base_url"] + "/chat/completions", data=payload,
        headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=CONFIG["lm_server"]["timeout_s"]) as resp:
        body = json.loads(resp.read().decode())
    code = body["choices"][0]["message"]["content"]
    py = Path(out).parent / f"{slug}__llamamesh.py"
    py.write_text(code)
    r = subprocess.run([CONFIG["blender_app"], "-b", "--python", str(py)],
                       capture_output=True, text=True, timeout=900)
    print(r.stdout[-1000:], file=sys.stderr)
    if r.returncode != 0:
        print(r.stderr[-1500:], file=sys.stderr)
        return r.returncode
    # LLaMA-Mesh costuma exportar no cwd; procura .glb recém-escrito e move.
    cands = sorted(Path(".").glob("*.glb"), key=lambda p: p.stat().st_mtime, reverse=True)
    if cands:
        shutil.copy2(cands[0], out)
        return 0
    print("[gen] LLaMA-Mesh não exportou .glb — ajuste o código gerado (o arquivo .py ficou salvo).")
    return 3


def run_trellis(slug: str, ref: Path, out: Path) -> int:
    """trellis2-mlx (repo oficial mlx): chama o entrypoint via subprocess.
    O repo mantém a interface CLI em engines/trellis2-mlx (ver README upstream);
    este wrapper valida a saída e sinaliza claramente se o adapter não estiver lá.
    """
    repo = ROOT / CONFIG["trellis"]["repo"]
    cli = repo / "cli_generate.py"
    if not cli.exists():
        print(f"[gen] adapter trellis ausente: {cli}\n"
              "      Instale `--with-trellis` (scripts/install.sh) para ativar a rota.")
        return 2
    t = CONFIG["trellis"]
    r = subprocess.run([
        sys.executable or "python", str(cli),
        "--input", str(ref), "--output", str(out),
        "--resolution", t["resolution"], "--texture-size", t["texture_size"],
        "--material-mode", t["material_mode"], "--mesh-repair", t["mesh_repair"],
        "--decimation", str(t["decimation_target"]),
    ], capture_output=True, text=True, timeout=2400)
    print(r.stdout[-1200:], file=sys.stderr)
    return r.returncode


def text_to_image(slug: str, prompt: str) -> Path | None:
    """Texto → imagem via ComfyUI (MLX Flux) quando o servidor está de pé."""
    import urllib.request
    url = CONFIG["comfyui"]["base_url"]
    try:
        urllib.request.urlopen(url + "/system_stats", timeout=5)
    except Exception:
        print("[gen] ComfyUI offline — a rota de texto exige a imagem de ref (use --ref).")
        return None
    wf = CONFIG["comfyui"].get("flux_workflow_json")
    if not wf:
        print("[gen] config comfyui.flux_workflow_json vazio — defina o workflow JSON "
              "(exporte do ComfyUI com o prompt de texto e aponte no config).")
        return None
    workflow = json.loads(Path(wf).read_text())
    # injeta o prompt no nó de texto (CLIPTextEncode positivo)
    for node in workflow.values():
        if node.get("class_type") in ("CLIPTextEncode", "CLIPTextEncodeFlux"):
            if node["inputs"].get("text", "").startswith("PROMPT"):
                node["inputs"]["text"] = prompt
    payload = json.dumps({"prompt": workflow}).encode()
    req = urllib.request.Request(url + "/prompt", data=payload,
                                 headers={"Content-Type": "application/json"})
    with urllib.request.urlopen(req, timeout=60) as resp:
        pid = json.loads(resp.read().decode()).get("prompt_id")
    print(f"[gen] ComfyUI job {pid}: aguardando… (verifique o histórico no ComfyUI)")
    return None  # retorno imediato; histórico no ComfyUI — usuário salva a ref em input/


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--slug", required=True)
    ap.add_argument("--prompt", required=True)
    ap.add_argument("--ref", type=Path, default=None)
    ap.add_argument("--route", default="auto", choices=["auto", "hy3d", "trellis", "llamamesh"])
    ap.add_argument("--tee", action="store_true", help="gera preview splat (tripoflux) antes")
    args = ap.parse_args()

    p = paths_for(args.slug)
    m = load_manifest(args.slug)
    hub = args.slug.split("_", 1)[0]
    if hub not in ("prop", "char", "creature", "env"):
        print(f"[gen] slug inválido: hub deve ser prop|char|creature|env — {args.slug}")
        return 2

    ref = args.ref
    if ref is None:
        ref = sorted(p["input"].glob(f"{args.slug}__ref*.png"))
        ref = ref[0] if ref else None
    if ref is None and args.route in ("hy3d", "trellis"):
        print("[gen] sem imagem de referência — gere uma (--ref ou ComfyUI flux) antes.")
        print("      Dica: salve em", p["input"])
        return 2

    prompt_file = p["input"] / f"{args.slug}__prompt.md"
    prompt_file.write_text(args.prompt)

    if args.route == "auto":
        route = "llamamesh" if (hub == "prop" and not ref) else "hy3d"
    else:
        route = args.route

    if args.tee:
        print("[gen] tee: tripoflux (blockout splat) pendente — adicione em engines/tripoflux-mlx "
              "e rode o preview manualmente (saída splat só serve p/ blockout).")

    out = p["draft"] / f"{args.slug}.draft.glb"
    if route == "hy3d":
        rc = run_hy3d(args.slug, ref, out)
    elif route == "trellis":
        rc = run_trellis(args.slug, ref, out)
    elif route == "llamamesh":
        rc = run_llamamesh(args.slug, args.prompt, out)
    else:
        rc = 2

    if rc == 0 and out.exists():
        m["status"] = "draft"
        m.setdefault("stages", {})["gen"] = {"route": route, "out": str(out),
                                             "bytes": out.stat().st_size}
        m["notes"].append(f"draft gerado via {route}")
        save_manifest(args.slug, m)
        print(f"[gen] OK -> {out}")
    else:
        m["status"] = "failed"
        save_manifest(args.slug, m)
    return rc


if __name__ == "__main__":
    sys.exit(main())
