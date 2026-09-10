'''Prepara uma malha gerada por IA para entrar no Unreal: decima, aplica escala, UV e exporta FBX.

Por que: o Hunyuan3D entrega malha densa (a primeira deu 1,7 milhao de faces) e o orcamento do proprio
pipeline para prop e 20 mil triangulos. Alem disso o UE quer UV e escala em centimetros.

Uso: Blender --background --python Tools/Blender/preparar_para_ue.py -- <glb> <fbx_saida> [alvo_tris]
'''

import sys

import bpy


def principal():
    argumentos = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    if len(argumentos) < 2:
        print('faltam argumentos: <glb> <fbx_saida> [alvo_tris]')
        return 2
    glb, saida = argumentos[0], argumentos[1]
    alvo = int(argumentos[2]) if len(argumentos) > 2 else 20000

    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=glb)

    malhas = [o for o in bpy.data.objects if o.type == 'MESH']
    antes = sum(len(o.data.polygons) for o in malhas)
    print(f'faces antes: {antes}')

    for objeto in malhas:
        bpy.context.view_layer.objects.active = objeto
        bpy.ops.object.select_all(action='DESELECT')
        objeto.select_set(True)

        # limpa e refaz a topologia antes de decimar (a malha da IA vem suja)
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.mesh.remove_doubles(threshold=0.0005)
        bpy.ops.mesh.normals_make_consistent(inside=False)
        bpy.ops.object.mode_set(mode='OBJECT')

        if len(objeto.data.polygons) > alvo:
            modificador = objeto.modifiers.new('decimar', 'DECIMATE')
            modificador.ratio = alvo / len(objeto.data.polygons)
            bpy.ops.object.modifier_apply(modifier=modificador.name)

        # UV: o UE precisa para material e lightmap
        if not objeto.data.uv_layers:
            bpy.ops.object.mode_set(mode='EDIT')
            bpy.ops.mesh.select_all(action='SELECT')
            bpy.ops.uv.smart_project(angle_limit=1.15, island_margin=0.02)
            bpy.ops.object.mode_set(mode='OBJECT')

        bpy.ops.object.shade_smooth()

    depois = sum(len(o.data.polygons) for o in malhas)
    print(f'faces depois: {depois}')

    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.export_scene.fbx(filepath=saida, use_selection=True, apply_scale_options='FBX_SCALE_ALL',
                             mesh_smooth_type='FACE', path_mode='COPY', embed_textures=False)
    print('exportado:', saida)
    return 0


if __name__ == '__main__':
    raise SystemExit(principal())
