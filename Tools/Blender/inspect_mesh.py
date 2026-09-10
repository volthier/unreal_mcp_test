"""Background Blender script: import a GLB and report object/mesh structure."""
import bpy, sys
from mathutils import Vector

GLB = sys.argv[sys.argv.index("--") + 1] if "--" in sys.argv else None

# Clean scene
bpy.ops.wm.read_factory_settings(use_empty=True)
if GLB:
    bpy.ops.import_scene.gltf(filepath=GLB)

print("\n===== SCENE =====")
for ob in bpy.data.objects:
    print(f"OBJ '{ob.name}' type={ob.type} parent={ob.parent.name if ob.parent else None}")
    if ob.type == 'MESH':
        me = ob.data
        print(f"  verts={len(me.vertices)} faces={len(me.polygons)} mats={[m.name for m in me.materials]}")
        print(f"  dims={tuple(round(v,3) for v in ob.dimensions)} loc={tuple(round(v,3) for v in ob.location)}")
        # world-space bbox height
        corners = [ob.matrix_world @ Vector(c) for c in ob.bound_box]
        zs = [c.z for c in corners]; xs=[c.x for c in corners]; ys=[c.y for c in corners]
        print(f"  world bbox: X[{min(xs):.3f},{max(xs):.3f}] Y[{min(ys):.3f},{max(ys):.3f}] Z[{min(zs):.3f},{max(zs):.3f}] height={max(zs)-min(zs):.3f}")
print("===== END =====")
