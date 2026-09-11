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
        # 0a. DESCARTA FACES GIGANTES. O Hunyuan3D reconstroi a CENA, e o fundo branco da imagem vira
        #     folha chapada em pe no telhado; o piso tambem vem como uma face enorme. Face grande demais
        #     e sempre artefato, nunca superficie do objeto - entao sai antes de tudo.
        malha_gigante = bmesh.new()
        malha_gigante.from_mesh(objeto.data)
        if malha_gigante.faces:
            xs = [v.co.x for v in malha_gigante.verts]
            ys = [v.co.y for v in malha_gigante.verts]
            zs = [v.co.z for v in malha_gigante.verts]
            diagonal = ((max(xs) - min(xs)) ** 2 + (max(ys) - min(ys)) ** 2 + (max(zs) - min(zs)) ** 2) ** 0.5
            limite = 0.05 * diagonal * diagonal
            gigantes = [f for f in malha_gigante.faces if f.calc_area() > limite]
            if gigantes:
                bmesh.ops.delete(malha_gigante, geom=gigantes, context='FACES')
                print('   faces gigantes descartadas:', len(gigantes))
            malha_gigante.to_mesh(objeto.data)
            objeto.data.update()
        malha_gigante.free()

        # 0. CORTA A LAJE DO CHAO, se ela vier junto. O Hunyuan3D reconstroi a CENA, e a imagem de
        #    referencia tinha o objeto sobre um piso: a malha sai com o piso colado (1,9 x 1,9 x 1,96
        #    num predio que e uma torre esguia). Sem cortar, no Unreal o predio vira uma laje gigante
        #    com a torre em cima.
        malha_limpeza = bmesh.new()
        malha_limpeza.from_mesh(objeto.data)
        alturas = [v.co.z for v in malha_limpeza.verts]
        if alturas:
            baixo, alto = min(alturas), max(alturas)
            corte = baixo + (alto - baixo) * 0.10
            descartar = [f for f in malha_limpeza.faces if all(v.co.z < corte for v in f.verts)]
            if descartar:
                bmesh.ops.delete(malha_limpeza, geom=descartar, context='FACES')
                print('   laje cortada:', len(descartar), 'faces')
            # e o pivo vai para a BASE: o predio assenta no chao sem conta de meia altura
            sobrou = [v.co.z for v in malha_limpeza.verts]
            if sobrou:
                deslocamento = min(sobrou)
                for v in malha_limpeza.verts:
                    v.co.z -= deslocamento
                print('   pivo movido para a base:', round(deslocamento, 3))
        malha_limpeza.to_mesh(objeto.data)
        malha_limpeza.free()
        objeto.data.update()

        # 1. solda vertices duplicados e corrige normais (bmesh: sem contexto de UI)
        malha = bmesh.new()
        malha.from_mesh(objeto.data)
        bmesh.ops.remove_doubles(malha, verts=malha.verts, dist=0.0005)
        bmesh.ops.recalc_face_normals(malha, faces=malha.faces)
        malha.to_mesh(objeto.data)
        malha.free()
        objeto.data.update()

        # 1a. SEPARA AS PARTES SOLTAS E FICA COM A MAIOR. O fundo branco da imagem vira uma folha em pe no
        #     telhado e o piso vira tiras soltas - tudo isso e GEOMETRIA DESCONECTADA do objeto. Separa e
        #     mantem a parte com mais faces (o predio). Filtro por tamanho de face nao resolve: essas
        #     folhas vem subdivididas em milhares de faces pequenas.
        bpy.context.view_layer.objects.active = objeto
        bpy.ops.object.select_all(action='DESELECT')
        objeto.select_set(True)
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.mesh.separate(type='LOOSE')
        bpy.ops.object.mode_set(mode='OBJECT')
        partes = [o for o in bpy.context.selected_objects if o.type == 'MESH']
        if len(partes) > 1:
            partes.sort(key=lambda o: len(o.data.polygons), reverse=True)
            principal, resto = partes[0], partes[1:]
            print('   partes soltas:', len(partes), '- ficando com', principal.name, 'e descartando', len(resto))
            for extra in resto:
                bpy.data.objects.remove(extra, do_unlink=True)
            objeto = principal
            bpy.context.view_layer.objects.active = objeto
            objeto.select_set(True)

        # 1b. ASSA O TRANSFORM NA MALHA. O glTF guarda a orientacao no NO do objeto e os dois
        #     exportadores (glTF x FBX) interpretam isso de formas diferentes: a torre saia DEITADA no
        #     Unreal (caixa 62 x 14 x 18 m, com o eixo longo em X, medido no proprio editor). Assando a
        #     matriz na malha e zerando o transform, os dois formatos concordam.
        if objeto.matrix_world != objeto.matrix_world.Identity(4):
            objeto.data.transform(objeto.matrix_world)
            objeto.matrix_world = objeto.matrix_world.Identity(4)
            objeto.data.update()
            print('   transform assado na malha')

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
