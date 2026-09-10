'''Prepara uma malha gerada por IA para entrar no Unreal: limpa, decima, UV, normais e exporta FBX.

Armadilhas que este script resolve (todas encontradas na pratica, no Blender 5.2 em background):
- o modificador Decimate adicionado + bpy.ops.object.modifier_apply NAO aplica em background:
  o contador de faces nao muda. Solucao: avaliar com depsgraph e trocar a malha (new_from_object).
- bpy.ops.mesh.decimate tambem e no-op em background. Nao usar operador de UI.
- mesh.use_auto_smooth nao existe mais no Blender 5.
- normais invertidas: o Unreal renderiza preto quando as faces apontam para dentro. A correcao aqui e
  por bmesh (recalc_face_normals), que nao depende de contexto de UI.

Uso: Blender --background --python Tools/Blender/preparar_para_ue.py -- <glb> <fbx> [alvo_tris]
'''

import sys

import bmesh
import bpy


def aplicar_decimacao(objeto, alvo):
    faces = len(objeto.data.polygons)
    if faces <= alvo:
        return faces
    modificador = objeto.modifiers.new('decimar', 'DECIMATE')
    modificador.ratio = max(0.001, alvo / faces)
    contexto = bpy.context.evaluated_depsgraph_get()
    malha_avaliada = bpy.data.meshes.new_from_object(objeto.evaluated_get(contexto))
    objeto.modifiers.clear()
    objeto.data = malha_avaliada
    return len(objeto.data.polygons)


def principal():
    argumentos = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    if len(argumentos) < 2:
        print('faltam argumentos: <glb> <fbx> [alvo_tris]')
        return 2
    glb, saida = argumentos[0], argumentos[1]
    alvo = int(argumentos[2]) if len(argumentos) > 2 else 20000

    bpy.ops.wm.read_factory_settings(use_empty=True)
    bpy.ops.import_scene.gltf(filepath=glb)

    malhas = [o for o in bpy.data.objects if o.type == 'MESH']
    print('faces antes:', sum(len(o.data.polygons) for o in malhas))

    for objeto in malhas:
        # 1. solda vertices duplicados e corrige normais (bmesh: sem contexto de UI)
        malha = bmesh.new()
        malha.from_mesh(objeto.data)
        bmesh.ops.remove_doubles(malha, verts=malha.verts, dist=0.0005)
        bmesh.ops.recalc_face_normals(malha, faces=malha.faces)
        malha.to_mesh(objeto.data)
        malha.free()
        objeto.data.update()

        # 2. decima pelo depsgraph
        print('  ', objeto.name, '->', aplicar_decimacao(objeto, alvo), 'faces')

        # 3. UV (o Unreal precisa para material e lightmap)
        if not objeto.data.uv_layers:
            bpy.context.view_layer.objects.active = objeto
            bpy.ops.object.select_all(action='DESELECT')
            objeto.select_set(True)
            bpy.ops.object.mode_set(mode='EDIT')
            bpy.ops.mesh.select_all(action='SELECT')
            bpy.ops.uv.smart_project(angle_limit=1.15, island_margin=0.02)
            bpy.ops.object.mode_set(mode='OBJECT')

    print('faces depois:', sum(len(o.data.polygons) for o in malhas))

    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.export_scene.fbx(filepath=saida, use_selection=True, apply_scale_options='FBX_SCALE_ALL',
                             mesh_smooth_type='FACE', path_mode='COPY', embed_textures=False)
    print('exportado:', saida)
    return 0


if __name__ == '__main__':
    raise SystemExit(principal())
