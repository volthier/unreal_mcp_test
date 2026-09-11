# -*- coding: utf-8 -*-
'''Importa a arte de tela gerada como TEXTURA no Content, para o menu usar como fundo.'''

import unreal

PASTA = '/Game/AI_Assets/ui'
ENTRADAS = [
    ('/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/cenario/login_kardys_v001.png', 'ui_login_kardys'),
    ('/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/cenario/pods_v002.png', 'ui_pods_selecao'),
]

saida = []

for caminho, nome in ENTRADAS:
    tarefa = unreal.AssetImportTask()
    tarefa.filename = caminho
    tarefa.destination_path = PASTA
    tarefa.destination_name = nome
    tarefa.automated = True
    tarefa.replace_existing = True
    tarefa.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
    saida.append(nome + ' -> ' + str(tarefa.imported_object_paths))

with open(unreal.Paths.project_saved_dir() + 'ImportarUI.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
