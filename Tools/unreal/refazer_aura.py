# -*- coding: utf-8 -*-
'''Refaz M_AuraGeada: a aura tem de ler como NEVOA, nao como bola acesa.

O defeito medido na captura: a casca esferica saiu como uma bola amarela solida. Causa: o emissivo estava
somando uma cor constante, sem a TEXTURA de geada no meio - e sem escala de UV a textura ficaria esticada
pela esfera inteira de qualquer forma. O que faz ler nevoa e: textura de geada em UV pequeno (repete muito)
vezes a cor do chassi, com brilho baixo, aditivo e dois lados.
'''

import unreal

PASTA = '/Game/AI_Assets/materials'
NOME = 'M_AuraGeada'
TEXTURA = '/Game/AI_Assets/ui/fx_nevoa_aura.fx_nevoa_aura'
saida = []

caminho = PASTA + '/' + NOME + '.' + NOME
antigo = unreal.EditorAssetLibrary.load_asset(caminho)
if antigo:
    unreal.EditorAssetLibrary.delete_asset(caminho)
    saida.append('material antigo apagado (estava somando cor sem textura)')

ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
mat = ferramentas.create_asset(NOME, PASTA, unreal.Material, unreal.MaterialFactoryNew())
mat.set_editor_property('blend_mode', unreal.BlendMode.BLEND_ADDITIVE)
mat.set_editor_property('shading_model', unreal.MaterialShadingModel.MSM_UNLIT)
mat.set_editor_property('two_sided', True)

# UV 1: UM puff por placa. Repetir o puff (7x) enche a placa de pontos, satura e ela vira quadrado - foi
# o que a captura mostrou. Com 1x, a propria queda do sprite apaga as bordas da placa e ela le como nevoa.
coordenadas = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionTextureCoordinate, -1400, 0)
escala = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant2Vector, -1400, 220)
escala.set_editor_property('r', 1.0)
escala.set_editor_property('g', 1.0)
multiplica_uv = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionMultiply, -1150, 100)
unreal.MaterialEditingLibrary.connect_material_expressions(coordenadas, '', multiplica_uv, 'A')
unreal.MaterialEditingLibrary.connect_material_expressions(escala, '', multiplica_uv, 'B')

# a nevoa se move devagar
panner = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionPanner, -950, 100)
panner.set_editor_property('speed_x', 0.015)
panner.set_editor_property('speed_y', 0.025)
unreal.MaterialEditingLibrary.connect_material_expressions(multiplica_uv, '', panner, 'Coordinate')

amostra = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D, -700, 100)
amostra.set_editor_property('parameter_name', 'TexturaDaAura')
textura = unreal.EditorAssetLibrary.load_asset(TEXTURA)
if textura:
    amostra.set_editor_property('texture', textura)
    saida.append('textura de geada ligada')
else:
    saida.append('ATENCAO: textura de geada nao encontrada')
unreal.MaterialEditingLibrary.connect_material_expressions(panner, '', amostra, 'UVs')

cor = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -700, 420)
cor.set_editor_property('parameter_name', 'CorDaAura')
cor.set_editor_property('default_value', unreal.LinearColor(0.55, 0.85, 1.0, 1.0))
brilho = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -700, 620)
brilho.set_editor_property('parameter_name', 'BrilhoDaAura')
brilho.set_editor_property('default_value', 0.35)
tinta = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionMultiply, -450, 500)
unreal.MaterialEditingLibrary.connect_material_expressions(cor, '', tinta, 'A')
unreal.MaterialEditingLibrary.connect_material_expressions(brilho, '', tinta, 'B')
final = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionMultiply, -200, 250)
unreal.MaterialEditingLibrary.connect_material_expressions(amostra, 'RGB', final, 'A')
unreal.MaterialEditingLibrary.connect_material_expressions(tinta, '', final, 'B')
unreal.MaterialEditingLibrary.connect_material_property(final, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)

unreal.MaterialEditingLibrary.recompile_material(mat)
unreal.EditorAssetLibrary.save_asset(caminho)
saida.append('M_AuraGeada refeito: geada em UV 7x, brilho 0.35, aditivo')

with open(unreal.Paths.project_saved_dir() + 'AuraRefeita.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
