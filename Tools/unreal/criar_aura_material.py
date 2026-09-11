# -*- coding: utf-8 -*-
'''Cria o material da AURA do nucleo: a nuvem de gelo que gira em volta do cristal.

Por que material e nao so Niagara: o sistema Niagara criado pela MCP ficou na memoria do editor (nao foi
salvo em disco), e o pipeline headless nao ve o que nao esta em disco. A casca de material com a textura de
geada entrega o efeito agora, com o sprite que o ComfyUI gerou, e o Niagara entra por cima quando estiver
salvo. Material Aditivo + Unlit + Panner: nevoa que rola, sem custo de particula.
'''

import unreal

PASTA = '/Game/AI_Assets/materials'
TEXTURA = '/Game/AI_Assets/ui/fx_geada.fx_geada'
NOME = 'M_AuraGeada'
saida = []


def carregar(caminho):
    return unreal.EditorAssetLibrary.load_asset(caminho) if unreal.EditorAssetLibrary.does_asset_exist(caminho) else None


material = carregar(PASTA + '/' + NOME)
if material is None:
    ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
    material = ferramentas.create_asset(NOME, PASTA, unreal.Material, unreal.MaterialFactoryNew())
    material.set_editor_property('blend_mode', unreal.BlendMode.BLEND_ADDITIVE)
    material.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_UNLIT)
    material.set_editor_property('two_sided', True)

    # textura da geada amostrada com UV que rola devagar (Panner) - a nevoa se move sozinha
    panner = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionPanner, -900, 0)
    panner.set_editor_property('speed_x', 0.03)
    panner.set_editor_property('speed_y', 0.05)
    amostra = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionTextureSampleParameter2D, -650, 0)
    amostra.set_editor_property('parameter_name', 'TexturaDaAura')
    textura = carregar(TEXTURA)
    if textura:
        amostra.set_editor_property('texture', textura)
    unreal.MaterialEditingLibrary.connect_material_expressions(panner, '', amostra, 'UVs')

    cor = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionVectorParameter, -650, 300)
    cor.set_editor_property('parameter_name', 'CorDaAura')
    cor.set_editor_property('default_value', unreal.LinearColor(0.55, 0.85, 1.0, 1.0))
    brilho = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionScalarParameter, -650, 500)
    brilho.set_editor_property('parameter_name', 'BrilhoDaAura')
    brilho.set_editor_property('default_value', 0.9)
    tinta = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionMultiply, -400, 400)
    unreal.MaterialEditingLibrary.connect_material_expressions(cor, '', tinta, 'A')
    unreal.MaterialEditingLibrary.connect_material_expressions(brilho, '', tinta, 'B')
    comTextura = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionMultiply, -200, 120)
    unreal.MaterialEditingLibrary.connect_material_expressions(amostra, 'RGB', comTextura, 'A')
    unreal.MaterialEditingLibrary.connect_material_expressions(tinta, '', comTextura, 'B')
    unreal.MaterialEditingLibrary.connect_material_property(comTextura, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    unreal.MaterialEditingLibrary.recompile_material(material)
    unreal.EditorAssetLibrary.save_asset(PASTA + '/' + NOME)
    saida.append('material da aura criado: ' + NOME)
else:
    saida.append('material da aura ja existia')

# a DataTable precisa ser reimportada COM o modulo novo (a struct ganhou CoreColor)
linha_struct = unreal.load_object(None, '/Script/PloidrekRPG.RunnerChassisData')
fabrica = unreal.CSVImportFactory()
fabrica.automated_import_settings.import_row_struct = linha_struct
tarefa = unreal.AssetImportTask()
tarefa.filename = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Data/DT_Chassis.csv'
tarefa.destination_path = '/Game/Data'
tarefa.destination_name = 'DT_Chassis'
tarefa.replace_existing = True
tarefa.automated = True
tarefa.save = True
tarefa.factory = fabrica
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
saida.append('DT_Chassis reimportada: ' + str(tarefa.imported_object_paths))

with open(unreal.Paths.project_saved_dir() + 'AuraMaterial.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
