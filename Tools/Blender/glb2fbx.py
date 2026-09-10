import bpy, sys, os
argv = sys.argv
idx = argv.index("--")
glb = argv[idx + 1]
out = argv[idx + 2]

bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.import_scene.gltf(filepath=glb)

# 1) shade smooth -> corrige "cartão chato" (normais suaves)
for ob in bpy.context.scene.objects:
    if ob.type == 'MESH':
        bpy.context.view_layer.objects.active = ob
        ob.select_set(True)
        try:
            bpy.ops.object.shade_smooth()
        except Exception as e:
            print("shade_smooth err", e)
        ob.select_set(False)

# 2) desempacotar texturas p/ FBX poder referenciá-las
try:
    bpy.ops.file.unpack_all(method='WRITE_LOCAL')
except Exception as e:
    print("unpack err", e)

# 3) exportar FBX com materiais; normais saem por padrão.
bpy.ops.object.select_all(action='SELECT')
base = dict(filepath=out, use_selection=True, object_types={'MESH'},
            apply_unit_scale=True, global_scale=1.0)
for extra in (dict(export_materials='EXPORT', path_mode='COPY'),
              dict(export_materials='EXPORT'),
              dict(),
              dict(apply_modifiers=True)):
    try:
        bpy.ops.export_scene.fbx(**base, **extra)
        break
    except TypeError as e:
        print("trying smaller params:", str(e)[:60])
print("fbx exported ->", out, "| dir:", os.path.dirname(out))
