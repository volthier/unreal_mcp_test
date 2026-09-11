# -*- coding: utf-8 -*-
'''Importa o conjunto da cidade: os dois predios e a textura do chao, e cria o material do piso.'''

import os
import unreal

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated'
saida = []


def importar(caminho, pasta, nome):
    if not os.path.exists(caminho):
        saida.append('sem arquivo: ' + os.path.basename(caminho))
        return
    tarefa = unreal.AssetImportTask()
    tarefa.filename = caminho
    tarefa.destination_path = pasta
    tarefa.destination_name = nome
    tarefa.automated = True
    tarefa.replace_existing = True
    tarefa.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
    saida.append('importado: ' + nome)


importar(RAIZ + '/3d/cristal_nucleo_game.fbx', '/Game/AI_Assets/prop', 'cristal_nucleo')
importar(RAIZ + '/3d/predio_alto_game.fbx', '/Game/AI_Assets/prop', 'predio_alto')
importar(RAIZ + '/3d/predio_baixo_game.fbx', '/Game/AI_Assets/prop', 'predio_baixo')
importar(RAIZ + '/mundo/chao_plaza_v001.png', '/Game/AI_Assets/ui', 'tex_chao_plaza')

# material do piso da praca: a textura do chao industrial, escura e com brilho de metal
caminho_mat = '/Game/AI_Assets/materials/M_ChaoDaCidade'
if not unreal.EditorAssetLibrary.does_asset_exist(caminho_mat):
    ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
    mat = ferramentas.create_asset('M_ChaoDaCidade', '/Game/AI_Assets/materials', unreal.Material, unreal.MaterialFactoryNew())
    if mat:
        textura = unreal.EditorAssetLibrary.load_asset('/Game/AI_Assets/ui/tex_chao_plaza.tex_chao_plaza')
        amostra = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D, -600, 0)
        amostra.set_editor_property('parameter_name', 'TexturaDoChao')
        if textura:
            amostra.set_editor_property('texture', textura)
        unreal.MaterialEditingLibrary.connect_material_property(amostra, 'RGB', unreal.MaterialProperty.MP_BASE_COLOR)
        rug = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -600, 300)
        rug.set_editor_property('parameter_name', 'Rugosidade')
        rug.set_editor_property('default_value', 0.42)
        unreal.MaterialEditingLibrary.connect_material_property(rug, '', unreal.MaterialProperty.MP_ROUGHNESS)
        met = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -600, 480)
        met.set_editor_property('parameter_name', 'Metalico')
        met.set_editor_property('default_value', 0.55)
        unreal.MaterialEditingLibrary.connect_material_property(met, '', unreal.MaterialProperty.MP_METALLIC)
        # a textura entra em escala grande: o piso da praca tem centenas de metros
        multiplica = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionMultiply, -350, -150)
        escala = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant2Vector, -550, -180)
        escala.set_editor_property('r', 24.0)
        escala.set_editor_property('g', 24.0)
        coordenadas = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -750, -180)
        unreal.MaterialEditingLibrary.connect_material_expressions(coordenadas, '', multiplica, 'A')
        unreal.MaterialEditingLibrary.connect_material_expressions(escala, '', multiplica, 'B')
        unreal.MaterialEditingLibrary.connect_material_expressions(multiplica, '', amostra, 'UVs')
        unreal.MaterialEditingLibrary.recompile_material(mat)
        unreal.EditorAssetLibrary.save_asset(caminho_mat)
        saida.append('material do chao criado: M_ChaoDaCidade')
else:
    saida.append('material do chao ja existia')

with open(unreal.Paths.project_saved_dir() + 'AssetsCidade.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
