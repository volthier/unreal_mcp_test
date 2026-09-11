# -*- coding: utf-8 -*-
'''Importa os predios da cidade (gerados no ComfyUI + tratados no Blender) para o Content.'''

import os
import unreal

PASTA = '/Game/AI_Assets/prop'
RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/3d'
saida = []

for nome in ('predio_alto', 'predio_baixo'):
    fbx = os.path.join(RAIZ, nome + '_game.fbx')
    if not os.path.exists(fbx):
        saida.append('sem FBX ainda: ' + nome)
        continue
    tarefa = unreal.AssetImportTask()
    tarefa.filename = fbx
    tarefa.destination_path = PASTA
    tarefa.destination_name = nome
    tarefa.automated = True
    tarefa.replace_existing = True
    tarefa.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
    saida.append('importado: ' + str(tarefa.imported_object_paths))

# e o grafo de PCG existe em disco? (a MCP cria na memoria do editor)
saida.append('grafo PCG em disco: ' + str(unreal.EditorAssetLibrary.does_asset_exist('/Game/AI_Assets/pcg/PCG_CidadeKardys')))

with open(unreal.Paths.project_saved_dir() + 'ImportarPredios.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
