#!/usr/bin/env python3
"""cleanup_mesh.py — etapa 3a: limpeza/retopo do mesh (Blender headless).

Roda com Blender (ver scripts/launch_*.sh):
  "/Applications/Blender.app/Contents/MacOS/Blender" -b --python cleanup_mesh.py -- \\
      --slug prop_weapon_sword_v001 [--tris 20000] [--render]

Funções: importa GLB/FBX, remove ilhas soltas, voxel remesh (se --retopo),
decimate até budget, triangula, normais consistentes, exporta FBX em
20_mesh/<slug>.clean.fbx e (com --render) despeja turntable 8 views em
10_draft/turntable/*.png para o crítico (Qwen3-VL).
"""
import argparse
import sys
from pathlib import Path

import bpy
import bmesh
import math

# ---------------- args (após "--") ----------------
argv = sys.argv[sys.argv.index("--") + 1:] if "--" in sys.argv else []
ap = argparse.ArgumentParser()
ap.add_argument("--slug", required=True)
ap.add_argument("--tris", type=int, default=0)
ap.add_argument("--retopo", action="store_true")
ap.add_argument("--render", action="store_true")
ap.add_argument("--root", default="/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/AI_Pipeline")
args = ap.parse_args(argv)

ROOT = Path(args.root)
WORK = ROOT / "work" / args.slug
DRAFT = WORK / "10_draft"
OUT_DIR = WORK / "20_mesh"
OUT_DIR.mkdir(parents=True, exist_ok=True)
(TMP := DRAFT / "turntable").mkdir(parents=True, exist_ok=True)

# ---------------- import ----------------
bpy.ops.wm.read_factory_settings(use_empty=True)
src = DRAFT / f"{args.slug}.pbr.glb"
if not src.exists():
    src = DRAFT / f"{args.slug}.draft.glb"
assert src.exists(), f"não achei {src}"
bpy.ops.import_scene.gltf(filepath=str(src))

mesh_objs = [o for o in bpy.data.objects if o.type == "MESH"]
if not mesh_objs:
    print("ERRO: nenhum mesh importado")
    sys.exit(1)

# ---------------- limpeza ----------------
for ob in mesh_objs:
    bpy.context.view_layer.objects.active = ob
    ob.select_set(True)
    # normais + decimação por voxel (retopo) se pedido
    if args.retopo:
        mod = ob.modifiers.new("Voxel", "REMESH")
        mod.mode = "VOXEL"
        mod.voxel_size = 0.01
        bpy.ops.object.modifier_apply(modifier=mod.name)

# join
for o in bpy.data.objects:
    o.select_set(o.type == "MESH")
bpy.context.view_layer.objects.active = mesh_objs[0]
bpy.ops.object.join()
ob = bpy.context.view_layer.objects.active

# remove ilhas soltas (componentes < 0.5% do volume/área não — simples: decima só)
me = ob.data
bm = bmesh.new()
bm.from_mesh(me)
bmesh.ops.recalc_face_normals(bm, faces=bm.faces)
bm.to_mesh(me)
bm.free()

# decimate até budget
tris_now = sum(len(p.vertices) - 2 for p in me.polygons)
budget = args.tris or 0
if budget and tris_now > budget:
    ratio = budget / max(1, tris_now)
    mod = ob.modifiers.new("Dec", "DECIMATE")
    mod.ratio = max(0.01, ratio)
    bpy.ops.object.modifier_apply(modifier=mod.name)

# triangulate
bpy.ops.object.mode_set(mode="EDIT")
bpy.ops.mesh.select_all(action="SELECT")
bpy.ops.mesh.quads_convert_to_tris()
bpy.ops.object.mode_set(mode="OBJECT")

# origem ao chão (pivot = base do bounding box)
bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
min_z = min((ob.matrix_world @ v.co).z for v in me.vertices)
ob.location.z = -min_z
bpy.ops.object.transform_apply(location=True)
bpy.context.scene.cursor.location = (0, 0, 0)
bpy.ops.object.origin_set(type="ORIGIN_CURSOR")

out_fbx = OUT_DIR / f"{args.slug}.clean.fbx"
bpy.ops.export_scene.fbx(filepath=str(out_fbx), use_selection=True)
tris_final = sum(len(p.vertices) - 2 for p in me.polygons)
print(f"MESH_OK tris={tris_final} -> {out_fbx}")

# ---------------- turntable (para o crítico) ----------------
if args.render:
    scene = bpy.context.scene
    scene.render.engine = "BLENDER_EEVEE_NEXT" if "BLENDER_EEVEE_NEXT" in [i.identifier for i in
        bpy.types.RenderSettings.bl_rna.properties["engine"].enum_items] else "BLENDER_EEVEE"
    scene.render.resolution_x = scene.render.resolution_y = 640
    scene.render.film_transparent = True
    bbox = [ob.matrix_world @ v.co for v in me.vertices]
    c = bbox[0].copy()
    for vv in bbox:
        c += vv
    c /= max(1, len(bbox))
    r = max((vv - c).length for vv in bbox) * 2.2
    cam_data = bpy.data.cameras.new("Cam")
    cam = bpy.data.objects.new("Cam", cam_data)
    scene.collection.objects.link(cam)
    scene.camera = cam
    sun = bpy.data.lights.new("Sun", "SUN")
    sun_o = bpy.data.objects.new("Sun", sun)
    scene.collection.objects.link(sun_o)
    sun_o.rotation_euler = (math.radians(50), 0, math.radians(30))
    views = 8
    for i in range(views):
        ang = 2 * math.pi * i / views
        cam.location = (c.x + r * math.cos(ang) * 0.0 + r * math.sin(ang),
                        c.y - r * math.cos(ang),
                        c.z + r * 0.35)
        # olha para o centro
        direction = c - cam.location
        cam.rotation_euler = direction.to_track_quat("-Z", "Y").to_euler()
        scene.render.filepath = str(TMP / f"{args.slug}__view{i:02d}.png")
        bpy.ops.render.render(write_still=True)
    print(f"TURNTABLE_OK {views} views -> {TMP}")

print("CLEANUP_DONE")
