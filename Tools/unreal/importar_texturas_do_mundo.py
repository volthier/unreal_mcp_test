# -*- coding: utf-8 -*-
'''Importa as TEXTURAS DO MUNDO (fachada, aco e chao) e confirma uma por uma.

Excecao EXC-005: o TextureTools.import_file do MCP recusa estes PNGs (mesmo sendo PNG valido). Este script e
o caminho que funciona - e ele CONFIRMA cada asset com does_asset_exist, porque a licao da rodada 13 foi
justamente essa: duas texturas nunca tinham entrado no projeto e ninguem percebeu.
'''

import os
import unreal

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/mundo'
PASTA = '/Game/AI_Assets/mundo'
ENTRADAS = [
    ('tex_fachada_latao.png', 'tex_fachada_latao'),
    ('tex_aco_escuro.png', 'tex_aco_escuro'),
    ('tex_terra.png', 'tex_terra'),
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

with open(unreal.Paths.project_saved_dir() + 'TexturasDoMundo.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
