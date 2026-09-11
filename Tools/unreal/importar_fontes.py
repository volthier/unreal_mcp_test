# -*- coding: utf-8 -*-
'''Importa as FONTES do guia (Orbitron e Exo 2) como assets de fonte, e confirma cada uma.

Excecao EXC-005 (a familia de imports que o MCP nao cobre): o import de FONTE precisa de factory propria,
entao vai pelo pipeline do engine - o mesmo caminho das texturas.

Fonte de origem: Google Fonts, licenca OFL. Pode ser usada e redistribuida dentro do projeto (ao contrario das
marcas dos botoes sociais, que o AGENTS.md proibe no Content).
'''

import os
import unreal

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/fontes'
PASTA = '/Game/AI_Assets/fonts'
ENTRADAS = [('Orbitron.ttf', 'F_Orbitron'), ('Exo2.ttf', 'F_Exo2')]
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

with open(unreal.Paths.project_saved_dir() + 'Fontes.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
