# -*- coding: utf-8 -*-
'''Importa os sprites da NEVOA DE GELO SECO (a pluma e a poca) e confirma cada um.

A pluma entra com o nome fx_nevoa_aura DE PROPOSITO: e o mesmo parametro TexturaDaAura que o material da aura
ja usa, entao a arte nova passa a valer sem mexer no material. Excecao EXC-005 (o import do MCP recusa estes
PNGs), e cada asset e confirmado com does_asset_exist.''

import os
import unreal

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/fx'
PASTA = '/Game/AI_Assets/ui'
ENTRADAS = [
    ('fumaca_seca_v001.png', 'fx_nevoa_aura'),
    ('fumaca_seca_poca_v001.png', 'fx_nevoa_poca'),
]
saida = []

for arquivo, nome in ENTRADAS:
    caminho = os.path.join(RAIZ, arquivo)
    if not os.path.exists(caminho):
        saida.append('sem arquivo: ' + arquivo)
        continue
    tarefa = unreal.AssetImportTask()
    tarefa.filename = caminho
    tarefa.destination_path = PASTA
    tarefa.destination_name = nome
    tarefa.automated = True
    tarefa.replace_existing = True
    tarefa.save = True
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
    existe = unreal.EditorAssetLibrary.does_asset_exist(PASTA + '/' + nome)
    saida.append('%s -> %s' % (nome, 'OK' if existe else 'FALHOU'))

with open(unreal.Paths.project_saved_dir() + 'FumacaSeca.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
