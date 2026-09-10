"""Background Blender: prepare + rig the cleric mesh (decimate, pivot, armature, auto-weights)."""
import bpy, sys
from mathutils import Vector

GLB = sys.argv[sys.argv.index("--") + 1]
OUT_BLEND = sys.argv[sys.argv.index("--") + 2]
TARGET_TRIS = 18000

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=GLB)

# --- find the mesh object
mesh_ob = next((o for o in bpy.data.objects if o.type == 'MESH'), None)
assert mesh_ob, "no mesh object"
me = mesh_ob.data
print(f"[prep] imported {mesh_ob.name} verts={len(me.vertices)} faces={len(me.polygons)}")

# --- decimate to a game-friendly density
mod = mesh_ob.modifiers.new("Decimate", 'DECIMATE')
ratio = max(0.01, min(1.0, TARGET_TRIS / max(1, len(me.polygons))))
mod.ratio = ratio
bpy.context.view_layer.objects.active = mesh_ob
bpy.ops.object.modifier_apply(modifier="Decimate")
print(f"[prep] decimated to {len(me.polygons)} faces (ratio {ratio:.3f})")

# --- move pivot so feet rest on Z=0 and centered in X/Y; apply scale/rot
bpy.ops.object.select_all(action='DESELECT')
mesh_ob.select_set(True)
bpy.context.view_layer.objects.active = mesh_ob
bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)
# world bbox
corners = [mesh_ob.matrix_world @ Vector(c) for c in mesh_ob.bound_box]
minz = min(c.z for c in corners)
cx = (min(c.x for c in corners) + max(c.x for c in corners)) / 2
cy = (min(c.y for c in corners) + max(c.y for c in corners)) / 2
mesh_ob.location.x -= cx
mesh_ob.location.y -= cy
mesh_ob.location.z -= minz
bpy.ops.object.transform_apply(location=True, rotation=False, scale=False)
print(f"[prep] pivot moved; new height={mesh_ob.dimensions.z:.3f}")

# --- create the armature
arm_data = bpy.data.armatures.new("ClericRig")
arm_ob = bpy.data.objects.new("ClericRig", arm_data)
bpy.context.collection.objects.link(arm_ob)
bpy.context.view_layer.objects.active = arm_ob
bpy.ops.object.mode_set(mode='EDIT')

H = mesh_ob.dimensions.z  # ~1.97
def bone(name, hx, hy, hz, tx, ty, tz, parent=None, connect=False):
    b = arm_data.edit_bones.new(name)
    b.head = Vector((hx, hy, hz))
    b.tail = Vector((tx, ty, tz))
    if parent:
        b.parent = arm_data.edit_bones[parent]
        b.use_connect = connect
    return b

# humanoid rig (units in meters; feet at Z=0)
bone("root",      0,0,0.00,   0,0,0.08, parent=None)
bone("pelvis",    0,0,0.10,   0,0,H*0.50, parent="root")
bone("spine",     0,0,H*0.50, 0,0,H*0.62, parent="pelvis", connect=True)
bone("chest",     0,0,H*0.62, 0,0,H*0.74, parent="spine", connect=True)
bone("neck",      0,0,H*0.74, 0,0,H*0.82, parent="chest", connect=True)
bone("head",      0,0,H*0.82, 0,0,H*1.00, parent="neck", connect=True)
# arms (slightly down = A-pose-ish)
for side, s in (("L", 1), ("R", -1)):
    bone(f"clavicle_{side}", s*0.06,0,H*0.72, s*0.14,0,H*0.70, parent="chest")
    bone(f"upperarm_{side}", s*0.14,0,H*0.70, s*0.26,0,H*0.62, parent=f"clavicle_{side}", connect=True)
    bone(f"lowerarm_{side}", s*0.26,0,H*0.62, s*0.38,0,H*0.60, parent=f"upperarm_{side}", connect=True)
    bone(f"hand_{side}",     s*0.38,0,H*0.60, s*0.44,0,H*0.60, parent=f"lowerarm_{side}", connect=True)
    bone(f"thigh_{side}",    s*0.08,0,H*0.45, s*0.09,0,H*0.22, parent="pelvis")
    bone(f"calf_{side}",     s*0.09,0,H*0.22, s*0.09,0,H*0.04, parent=f"thigh_{side}", connect=True)
    bone(f"foot_{side}",     s*0.09,0,H*0.04, s*0.09,0.10,H*0.02, parent=f"calf_{side}", connect=True)

bpy.ops.object.mode_set(mode='OBJECT')

# --- parent mesh to armature with automatic weights
bpy.ops.object.select_all(action='DESELECT')
mesh_ob.select_set(True)
arm_ob.select_set(True)
bpy.context.view_layer.objects.active = arm_ob
try:
    bpy.ops.object.parent_set(type='ARMATURE_AUTO')
    print(f"[rig] automatic weights applied")
except Exception as e:
    print(f"[rig] AUTO WEIGHTS FAILED: {e}; falling back to envelope")
    bpy.ops.object.parent_set(type='ARMATURE_ENVELOPE')

# save
bpy.ops.wm.save_as_mainfile(filepath=OUT_BLEND)
print(f"[prep] saved {OUT_BLEND}")
print("[done]")
