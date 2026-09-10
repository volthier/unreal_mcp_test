"""Background Blender: export the rigged + animated cleric as an FBX for UE."""
import bpy, sys

BLEND = sys.argv[sys.argv.index("--") + 1]
OUT = sys.argv[sys.argv.index("--") + 2]

bpy.ops.wm.open_mainfile(filepath=BLEND)

bpy.ops.object.select_all(action='DESELECT')
for ob in bpy.data.objects:
    if ob.type in ('ARMATURE', 'MESH'):
        ob.select_set(True)
bpy.context.view_layer.objects.active = bpy.data.objects.get("ClericRig")

kwargs = dict(
    filepath=OUT,
    use_selection=True,
    object_types={'ARMATURE', 'MESH'},
    add_leaf_bones=False,
    bake_anim=True,
    apply_unit_scale=True,
    global_scale=1.0,
    primary_bone_axis='Y',
    secondary_bone_axis='X',
)
# Blender 5.2 FBX exporter animation flags.
for k, v in (("bake_anim_use_nla_strips", True), ("bake_anim_use_all_actions", True),
             ("bake_anim_use_all_bones", True), ("bake_anim_force_startend_keying", True),
             ("bake_anim_simplify_factor", 1.0)):
    try:
        kwargs[k] = v
    except Exception:
        pass

try:
    bpy.ops.export_scene.fbx(**kwargs)
    print(f"[fbx] exported {OUT}", flush=True)
except TypeError as e:
    print(f"[fbx] retry (TypeError: {e})", flush=True)
    kwargs.pop("bake_anim_use_all_actions", None)
    bpy.ops.export_scene.fbx(**kwargs)
    print(f"[fbx] exported (retry) {OUT}", flush=True)
print("[done]", flush=True)
