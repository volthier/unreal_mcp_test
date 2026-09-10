#!/usr/bin/env python3
"""mocap_retarget.py — etapa 4b: landmarks MediaPipe → Rigify (Blender headless).

Roda com Blender:
  Blender -b --python mocap_retarget.py -- --slug char_player_knight_v001 \
      --action walk [--json work/<slug>/input/<slug>__mocap_walk.json]

Mapeia 33 landmarks do MediaPipe para os ossos do rig (Rigify human) com
fallback geométrico: para cada frame, ossos-chave (pélvis→ombros→cotovelos→
pulsos, quadril→joelhos→tornozelos, cabeça) são orientados pelos vetores entre
marco proximal/distal via rotation_difference. Bake em action + export FBX em
40_anim/<slug>.anim_{action}_001.fbx (com esqueleto e malha — sem retarget no UE).
"""
import argparse
import json
import math
import sys
from pathlib import Path

import bpy
from mathutils import Vector

argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
ap = argparse.ArgumentParser()
ap.add_argument("--slug", required=True)
ap.add_argument("--action", required=True)
ap.add_argument("--json", type=Path, default=None)
ap.add_argument("--root", default="/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/AI_Pipeline")
args = ap.parse_args(argv)

ROOT = Path(args.root)
WORK = ROOT / "work" / args.slug
json_path = args.json or next(WORK.joinpath("input").glob(f"{args.slug}__mocap_{args.action}.json"), None)
assert json_path and json_path.exists(), f"falta JSON de captura ({args.slug}__mocap_{args.action}.json)"
data = json.loads(json_path.read_text())

RIG_FBX = WORK / "30_rig" / f"{args.slug}.rig.fbx"
assert RIG_FBX.exists(), f"falta {RIG_FBX} (rode rigify_rig.py antes)"
OUT_DIR = WORK / "40_anim"
OUT_DIR.mkdir(parents=True, exist_ok=True)

# MediaPipe Pose landmark ids → (osso, id proximal, id distal)
# HumanPose: 0 nose, 11/12 ombros, 13/14 cotovelos, 15/16 pulsos,
# 23/24 quadril, 25/26 joelhos, 27/28 tornozelos, 31/32 pés.
MAP = [
    # (bone_name, landmark_proximal, landmark_distal)
    ("upper_arm.L", 11, 13), ("forearm.L", 13, 15), ("upper_arm.R", 12, 14),
    ("forearm.R", 14, 16), ("thigh.L", 23, 25), ("shin.L", 25, 27),
    ("thigh.R", 24, 26), ("shin.R", 26, 28), ("spine", 23, 11),
    ("spine.001", 11, 0), ("spine.002", 11, 0), ("head", 0, 8),
]

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.fbx(filepath=str(RIG_FBX))
rig = next(o for o in bpy.data.objects if o.type == "ARMATURE")
mesh = next(o for o in bpy.data.objects if o.type == "MESH")

# normalize: pose no espaço de imagem (o frame tem pessoas com alturas variáveis) — usa
# a distância quadril-ombro como unidade (unidade do MediaPipe = 0..1, y = baixo p/ cima)
frames = [f for f in data["frames"] if f["lm"] and any(p[3] > 0.4 for p in f["lm"])]
if len(frames) < 5:
    print("ERRO: poucos frames com pose confiável — refaça a captura com mais luz/espaço.")
    sys.exit(1)

fps = max(1, int(data.get("fps", 30)))
scene = bpy.context.scene
scene.render.fps = fps

bpy.context.view_layer.objects.active = rig
bpy.ops.object.mode_set(mode="POSE")
for pb in rig.pose.bones:
    pb.rotation_mode = "QUATERNION"

def frame_bones(fr):
    """orienta cada osso pelo vetor entre landmarks (proximal→distal)."""
    lms = fr["lm"]
    for bone_name, pid, did in MAP:
        if bone_name not in rig.pose.bones:
            continue
        try:
            a = Vector((lms[pid][0], 1 - lms[pid][1], -lms[pid][2]))
            b = Vector((lms[did][0], 1 - lms[did][1], -lms[did][2]))
        except IndexError:
            continue
        if a.length < 1e-4 or b.length < 1e-4:
            continue
        bone = rig.pose.bones[bone_name]
        vec = (b - a)
        if vec.length < 1e-6:
            continue
        # quaternion que gira +Y do osso para o vetor (bone aponta para o filho)
        q = Vector((0, 1, 0)).rotation_difference(vec.normalized())
        bone.rotation_quaternion = q
    return True

for idx, fr in enumerate(frames):
    frame_bones(fr)
    scene.frame_set(idx)
    for pb in rig.pose.bones:
        pb.keyframe_insert("rotation_quaternion", frame=idx)

bpy.ops.object.mode_set(mode="OBJECT")
from mathutils import Quaternion
act = bpy.data.actions.get(f"{args.slug}_anim_{args.action}") or bpy.data.actions.new(
    f"{args.slug}_anim_{args.action}")
if rig.animation_data is None:
    rig.animation_data_create()
rig.animation_data.action = act
scene.frame_end = max(1, len(frames))

out = OUT_DIR / f"{args.slug}.anim_{args.action}_001.fbx"
bpy.ops.object.select_all(action="DESELECT")
rig.select_set(True)
mesh.select_set(True)
bpy.context.view_layer.objects.active = rig
bpy.ops.export_scene.fbx(filepath=str(out), use_selection=True, bake_anim=True)
print(f"RETARGET_OK -> {out} (frames={len(frames)}, fps={fps})")
print("MOCAP_DONE")
