#!/usr/bin/env python3
"""qc.py — gates de qualidade numérica do pipeline.

Aceita GLB/FBX/OBJ (GLB lido com trimesh; FBX/OBJ delegados a um Blender
headless quando o filetype não é suportado). Escreve um JSON de métricas e
retorna exit code != 0 quando algum gate do hub falha.

Uso:
  qc.py --asset work/<slug>/50_game/<slug>.game.lod0.fbx --hub prop [--json]
Gates configurados em config/pipeline.json -> qcs.*
"""
import argparse
import json
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = json.loads((ROOT / "config" / "pipeline.json").read_text())
QCS = CONFIG["qcs"]


def _metrics_glb(path: Path) -> dict:
    import trimesh
    m = trimesh.load_mesh(str(path), process=True)
    tris = len(m.faces)
    verts = len(m.vertices)
    boundary = 0
    if hasattr(m, "is_watertight") and m.is_watertight is not None:
        boundary = 0 if m.is_watertight else -1
    # texturas
    textures = []
    try:
        for tex in getattr(m.visual, "material", type("x", (), {"image": None})()).__dict__.get("images", []) or []:
            if tex is not None:
                textures.append(max(tex.size))
    except Exception:
        textures = []
    return {
        "format": "glb", "tris": int(tris), "verts": int(verts),
        "boundary_edges": boundary, "is_watertight": bool(boundary == 0),
        "texture_max_dim": max(textures) if textures else 0,
        "file_bytes": path.stat().st_size,
    }


def _metrics_fbx_via_blender(path: Path, hub: str) -> dict:
    """FBX/OBJ: delega a um mini-script Blender headless que reporta métricas."""
    probe = f"""
import bpy, json, sys
path = sys.argv[-2]; out = sys.argv[-1]
bpy.ops.wm.read_factory_settings(use_empty=True)
if path.lower().endswith('.fbx'):
    bpy.ops.import_scene.fbx(filepath=path)
else:
    bpy.ops.wm.obj_import(filepath=path)
import bmesh
dg = bpy.context.evaluated_depsgraph_get()
total_tris = total_verts = 0; bones = 0; anim_dur = 0.0
for ob in bpy.data.objects:
    if ob.type == 'MESH':
        ob_eval = ob.evaluated_get(dg)
        me = ob_eval.to_mesh()
        bm = bmesh.new(); bm.from_mesh(me)
        total_tris += sum(len(f.verts) - 2 for f in bm.faces)
        total_verts += len(bm.verts)
        bm.free()
        ob_eval.to_mesh_clear()
    elif ob.type == 'ARMATURE':
        bones = len(ob.data.bones)
        if ob.animation_data and ob.animation_data.action:
            anim_dur = ob.animation_data.action.frame_range[1] / max(24.0, 1.0)
json.dump({{"format": str(Path(path).suffix), "tris": int(total_tris),
          "verts": int(total_verts), "bones": int(bones),
          "anim_duration_s": round(anim_dur, 2)}}, open(out, "w"))
"""
    blender = CONFIG["blender_app"]
    if not Path(blender).exists():
        print(f"[qc] Blender não encontrado em {blender} — instale antes (ver README).")
        return {"format": str(path.suffix), "error": "blender_missing"}
    with tempfile.TemporaryDirectory(dir=ROOT / "tmp") as td:
        src = Path(td) / "probe.py"
        out = Path(td) / "metrics.json"
        src.write_text(probe)
        r = subprocess.run([blender, "-b", "--python", str(src), "--", str(path), str(out)],
                           capture_output=True, text=True, timeout=600)
        if r.returncode != 0 or not out.exists():
            return {"format": str(path.suffix), "error": "blender_probe_failed",
                    "stderr": r.stderr[-500:]}
        return json.loads(out.read_text())


def evaluate(metrics: dict, hub: str) -> tuple[bool, list[str]]:
    fails = []
    budget = QCS["hub_budgets_tris"].get(hub)
    if budget and metrics.get("tris", 0) > budget:
        fails.append(f"tris {metrics['tris']} > budget {budget} ({hub})")
    if "texture_max_dim" in metrics and metrics["texture_max_dim"] < QCS["texture_min_res"]:
        fails.append(f"textura {metrics['texture_max_dim']} < {QCS['texture_min_res']}")
    if metrics.get("is_watertight") is False:
        fails.append("mesh não é watertight")
    bones = metrics.get("bones", 0)
    if bones == -1:
        pass
    elif metrics.get("bones", -1) > 0 and bones < QCS["min_bones"]:
        fails.append(f"bones {bones} < {QCS['min_bones']}")
    if metrics.get("anim_duration_s") is not None:
        if 0 < metrics["anim_duration_s"] < QCS["min_anim_duration_s"]:
            fails.append(f"animação {metrics['anim_duration_s']}s < {QCS['min_anim_duration_s']}s")
    return (not fails), fails


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--asset", required=True, type=Path)
    ap.add_argument("--hub", required=True, choices=["prop", "char", "creature", "env"])
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()

    path = args.asset.resolve()
    if not path.exists():
        print(f"[qc] arquivo não existe: {path}")
        return 1
    if path.suffix.lower() == ".glb":
        metrics = _metrics_glb(path)
    else:
        metrics = _metrics_fbx_via_blender(path, args.hub)
    if "error" in metrics:
        print(f"[qc] {metrics['error']}: {metrics.get('stderr', '')}")
        return 2

    ok, fails = evaluate(metrics, args.hub)
    report = {"asset": str(path), "hub": args.hub, "metrics": metrics, "pass": ok, "failures": fails}
    if args.json:
        print(json.dumps(report, indent=2))
    else:
        print(f"pass={ok} tris={metrics.get('tris')} verts={metrics.get('verts')} "
              f"bones={metrics.get('bones')} textures={metrics.get('texture_max_dim')}")
        for f in fails:
            print(f"  FALHOU: {f}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
