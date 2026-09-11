# -*- coding: utf-8 -*-
'''Cria o material do NUCLEO (parametrico) e uma instancia por chassi, na cor de cada um.

Design (Docs/Prompts_Arte_Ploidrek.md 9b): o nucleo tem cor FIXA por chassi, ligada a habilidade - o
jogador escolhe as cores do CORPO, nunca a do nucleo. Entao o material e um so, parametrico, e cada
chassi tem sua instancia: trocar a cor de um chassi e trocar uma instancia, nao o material.
'''

import csv
import unreal

PASTA = '/Game/AI_Assets/materials'
BASE_NOME = 'M_NucleoParam'
saida = []


def carregar(caminho):
    return unreal.EditorAssetLibrary.load_asset(caminho) if unreal.EditorAssetLibrary.does_asset_exist(caminho) else None


# 1. material base com parametros (cor do nucleo + brilho + rugosidade)
caminho_base = PASTA + '/' + BASE_NOME
base = carregar(caminho_base)
if base is None:
    ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
    base = ferramentas.create_asset(BASE_NOME, PASTA, unreal.Material, unreal.MaterialFactoryNew())
    cor = unreal.MaterialEditingLibrary.create_material_expression(base, unreal.MaterialExpressionVectorParameter, -700, -200)
    cor.set_editor_property('parameter_name', 'CorDoNucleo')
    cor.set_editor_property('default_value', unreal.LinearColor(0.35, 0.75, 1.0, 1.0))
    unreal.MaterialEditingLibrary.connect_material_property(cor, '', unreal.MaterialProperty.MP_BASE_COLOR)
    # emissivo: a cor multiplicada por um brilho parametrizado, para o nucleo parecer aceso
    brilho = unreal.MaterialEditingLibrary.create_material_expression(base, unreal.MaterialExpressionScalarParameter, -700, 200)
    brilho.set_editor_property('parameter_name', 'Brilho')
    brilho.set_editor_property('default_value', 0.55)
    multiplica = unreal.MaterialEditingLibrary.create_material_expression(base, unreal.MaterialExpressionMultiply, -450, 100)
    unreal.MaterialEditingLibrary.connect_material_expressions(cor, '', multiplica, 'A')
    unreal.MaterialEditingLibrary.connect_material_expressions(brilho, '', multiplica, 'B')
    unreal.MaterialEditingLibrary.connect_material_property(multiplica, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    rug = unreal.MaterialEditingLibrary.create_material_expression(base, unreal.MaterialExpressionScalarParameter, -700, 450)
    rug.set_editor_property('parameter_name', 'Rugosidade')
    rug.set_editor_property('default_value', 0.15)
    unreal.MaterialEditingLibrary.connect_material_property(rug, '', unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.recompile_material(base)
    unreal.EditorAssetLibrary.save_asset(caminho_base)
    saida.append('material base criado: ' + BASE_NOME)
else:
    saida.append('material base ja existia')

# 2. uma instancia por chassi, com a cor do CSV (a fonte da verdade)
with open('/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Data/DT_Chassis.csv', encoding='utf-8-sig') as arquivo:
    chassis = list(csv.DictReader(arquivo))

ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
for linha in chassis:
    nome = linha['Name']
    texto = linha.get('CoreColor') or ''
    valores = {}
    for parte in texto.strip().strip('()').split(','):
        if '=' in parte:
            chave, valor = parte.split('=', 1)
            valores[chave.strip()] = float(valor)
    cor = unreal.LinearColor(valores.get('R', 1.0), valores.get('G', 1.0), valores.get('B', 1.0), 1.0)
    nome_instancia = 'MI_Nucleo_' + nome
    caminho = PASTA + '/' + nome_instancia
    instancia = carregar(caminho)
    if instancia is None:
        instancia = ferramentas.create_asset(nome_instancia, PASTA, unreal.MaterialInstanceConstant, unreal.MaterialInstanceConstantFactoryNew())
    if instancia is None:
        saida.append('FALHOU a instancia de ' + nome)
        continue
    unreal.MaterialEditingLibrary.set_material_instance_parent(instancia, base)
    # SEMPRE escreve os parametros, inclusive em instancia que ja existia: senao um ajuste de brilho nunca
    # chega nas instancias antigas (foi o que deixou o cristal estourado depois de eu baixar o valor).
    unreal.MaterialEditingLibrary.set_material_instance_vector_parameter_value(instancia, 'CorDoNucleo', cor)
    unreal.MaterialEditingLibrary.set_material_instance_scalar_parameter_value(instancia, 'Brilho', 0.55)
    unreal.EditorAssetLibrary.save_asset(caminho)
    saida.append('instancia: ' + nome_instancia + ' cor (' + str(round(cor.r, 2)) + ', ' + str(round(cor.g, 2)) + ', ' + str(round(cor.b, 2)) + ')')

with open(unreal.Paths.project_saved_dir() + 'MateriaisNucleo.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
