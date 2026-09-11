# -*- coding: utf-8 -*-
'''Confere no engine se o CoreColor chegou na DataTable e importa a arte nova (nuvens + geada).'''

import unreal

saida = []

tabela = unreal.EditorAssetLibrary.load_asset('/Game/Data/DT_Chassis')
if tabela:
    nomes = unreal.DataTableFunctionLibrary.get_data_table_row_names(tabela)
    for nome in list(nomes)[:12]:
        linha = unreal.DataTableFunctionLibrary.get_data_table_row_from_name(tabela, nome)
        if linha:
            saida.append(str(nome) + ' -> core ' + str(linha.get_editor_property('core_color')))
else:
    saida.append('DT_Chassis NAO encontrada')

for caminho, nome in [
    ('/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/cenario/nuvens_ocre_v001.png', 'ui_nuvens_ocre'),
    ('/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/fx/geada_v001.png', 'fx_geada'),
]:
    tarefa = unreal.AssetImportTask()
    tarefa.filename = caminho
    tarefa.destination_path = '/Game/AI_Assets/ui'
    tarefa.destination_name = nome
    tarefa.automated = True
    tarefa.replace_existing = True
    tarefa.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
    saida.append('importado: ' + nome)

with open(unreal.Paths.project_saved_dir() + 'CoreColor.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
