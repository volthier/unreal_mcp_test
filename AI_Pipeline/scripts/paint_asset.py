#!/usr/bin/env python3
"""paint_asset.py — etapa 2: texturização PBR (Hunyuan3D Stage 2, MLX).

Entrada: mesh (GLB) + imagem de referência → saída: <slug>.pbr.glb com
albedo (4096²) + metallicRoughness embutidos.

Uso:
  paint_asset.py --slug prop_weapon_sword_v001 [--mesh path] [--ref path]
"""
import argparse
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = json.loads((ROOT / "config" / "pipeline.json").read_text())


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--slug", required=True)
    ap.add_argument("--mesh", type=Path, default=None)
    ap.add_argument("--ref", type=Path, default=None)
    ap.add_argument("--env", default="hy3d-mlx")
    args = ap.parse_args()

    work = ROOT / "work" / args.slug
    draft = work / "10_draft"
    mesh = args.mesh or (draft / f"{args.slug}.draft.glb")
    ref = args.ref or next(sorted(work.joinpath("input").glob(f"{args.slug}__ref*.png")), None)
    if not mesh.exists() or ref is None:
        print(f"[paint] faltando mesh ({mesh}) ou imagem de referência — rode gen_asset.py antes.")
        return 2

    s2 = CONFIG["hunyuan3d"]["stage2"]
    out = draft / f"{args.slug}.pbr.glb"
    code = f"""
import sys, json
sys.path.insert(0, {str(ROOT / "engines" / "hunyuan3d-2.1-mlx" / "hy3dpaint")!r})
from textureGenPipeline_mlx import Hunyuan3DPaintConfigMLX, Hunyuan3DPaintPipelineMLX
cfg = Hunyuan3DPaintConfigMLX(max_num_view={s2["max_num_view"]}, resolution={s2["resolution"]})
pipe = Hunyuan3DPaintPipelineMLX(cfg)
pipe(
    mesh_path={str(mesh.resolve())!r},
    image_path={str(ref.resolve())!r},
    output_mesh_path={str(draft.resolve() / f"{args.slug}__untextured.obj")!r},
    save_glb=True,
)
import shutil
shutil.copy({str(draft.resolve() / f"{args.slug}__untextured.obj")!r}, {str(out.resolve())!r})
print("PAINT_DONE")
"""
    script = draft / f"{args.slug}__st2.py"
    script.write_text(code)

    import os
    python = Path(os.path.expanduser(f"~/miniforge3/envs/{args.env}/bin/python"))
    if not python.exists():
        print(f"[paint] env {args.env} não encontrado (rode scripts/install.sh).")
        return 2
    r = subprocess.run([str(python), str(script)], capture_output=True, text=True, timeout=2700)
    print(r.stdout[-1500:], file=sys.stderr)
    if r.returncode != 0:
        print(r.stderr[-3000:], file=sys.stderr)
        return r.returncode

    m_path = ROOT / "work" / "manifests" / f"{args.slug}.json"
    if m_path.exists():
        m = json.loads(m_path.read_text())
        m["status"] = "textured"
        m.setdefault("stages", {})["paint"] = {"out": str(out), "config": s2}
        m_path.write_text(json.dumps(m, indent=2, ensure_ascii=False))
    print(f"[paint] OK -> {out}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
