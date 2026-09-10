#!/usr/bin/env python3
"""rigify_rig.py — etapa 3b: rig automático via Rigify (Blender headless).

Roda com Blender:
  Blender -b --python rigify_rig.py -- --slug char_player_knight_v001
     [--kind human|basic] [--cleanup]

Fluxo: importa <slug>.clean.fbx → cria meta-rig (Rigify human ou basic) →
alinhamento (raiz ao chão, escala pelo bbox) → parent com automatic weights →
aplica rigify (gerar) → exporta <slug>.rig.fbx (armature + skinned mesh).

Nota: em Mac não há modelo HF MLX/MPS de rig — Rigify é o garantido local.
Para criaturas multi-membro, use o modo --cleanup e o auto-weights do Blender
(bone heat) e ajuste manualmente; MagicArticulate (CUDA/CPU-fallback) fica
documentado como opção lenta no README.
"""
import argparse
import sys
from pathlib import Path

import bpy

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
ap = argparse.ArgumentParser()
ap.add_argument("--slug", required=True)
ap.add_argument("--kind", default="human", choices=["human", "basic"])
ap.add_argument("--root", default="/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/AI_Pipeline")
args = ap.parse_args(argv)

ROOT = Path(args.root)
WORK = ROOT / "work" / args.slug
SRC = WORK / "20_mesh" / f"{args.slug}.clean.fbx"
OUT_DIR = WORK / "30_rig"
OUT_DIR.mkdir(parents=True, exist_ok=True)
assert SRC.exists(), f"falta {SRC} (rode cleanup_mesh.py antes)"

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=str(SRC))
mesh = next(o for o in bpy.data.objects if o.type == "MESH")

# habilita rigify (vem embutido, mas off por default)
import addon_utils
for mod_name in ("rigify",):
    try:
        addon_utils.enable(mod_name, default_set=True, persistent=True)
    except Exception as exc:
        print(f"AVISO: addon rigify: {exc}")

# meta-rig
bpy.ops.object.armature_human_metarig_add() if args.kind == "human" else \
    bpy.ops.object.armature_basic_metarig_add()
rig = bpy.context.view_layer.objects.active
rig.name = f"{args.slug}_rig"

# escala do rig pelo bbox do mesh (Rigify human parte de ~1.8m)
mesh.update_from_editmode()
coords = [mesh.matrix_world @ v.co for v in mesh.data.vertices]
height = max(c.z for c in coords) - min(c.z for c in coords)
scale = height / 1.8 if args.kind == "human" else height / 2.0
rig.scale = (scale, scale, scale)
bpy.context.view_layer.update()

# parent com pesos automáticos (bone heat) — mesh precisa estar selecionado
for o in bpy.data.objects:
    o.select_set(False)
mesh.select_set(True)
rig.select_set(True)
bpy.context.view_layer.objects.active = rig
try:
    bpy.ops.object.parent_set(type="ARMATURE_AUTO")
except RuntimeError as exc:
    print(f"AVISO: bone heat falhou ({exc}) — usando ARMATURE_NAME (sem pesos automáticos) "
          "e sugerindo: painel Weight > normalize + brush em: Blender GUI")
    bpy.ops.object.parent_set(type="ARMATURE_NAME")

# aplica rigify (gera o skeleton final + control rig)
try:
    bpy.ops.pose.rigify_generate()
except Exception as exc:
    print(f"AVISO: rigify_generate falhou: {exc} (o meta-rig já serve como esqueleto de exportação)")

out = OUT_DIR / f"{args.slug}.rig.fbx"
bpy.ops.object.select_all(action="DESELECT")
for o in bpy.data.objects:
    o.select_set(o.type in ("ARMATURE", "MESH"))
bpy.ops.export_scene.fbx(filepath=str(out), use_selection=True)
print(f"RIG_OK -> {out} (bones={len(rig.data.bones) if rig.data.bones else 'meta-rig'})")
print("RIGIFY_DONE")
