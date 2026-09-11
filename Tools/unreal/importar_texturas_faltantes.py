# -*- coding: utf-8 -*-
'''Importa as texturas que faltavam: a geada da aura e a nuvem ocre do fundo do login.

Achado na rodada 13: o material da aura somava cor SEM textura (causa da 'bola solida') porque a textura de
geada nunca tinha sido importada - o script da rodada 3 falhou no meio e ninguem conferiu. A nuvem ocre
estava no mesmo caso, e ela e o FUNDO DO LOGIN: a configuracao apontava para um asset que nao existia.
'''

import os
import unreal

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated'
PASTA = '/Game/AI_Assets/ui'
ENTRADAS = [
    (RAIZ + '/fx/geada_v001.png', 'fx_geada'),
    (RAIZ + '/fx/nevoa_aura_v002.png', 'fx_nevoa_aura'),
    (RAIZ + '/cenario/login_cidade_escolhido.png', 'ui_nuvens_ocre'),   # a cidade de Kardys ao crepusculo
    (RAIZ + '/cenario/login_kardys_v001.png', 'ui_login_kardys'),
    (RAIZ + '/cenario/pods_v002.png', 'ui_pods_selecao'),
    # Kit visual AAA do menu (EXC-005): o TextureTools.import_file do MCP recusa estes PNGs
    (RAIZ + '/ui/kit_moldura_v001.png', 'ui_moldura_latao'),
    (RAIZ + '/ui/kit_vidro_v001.png', 'ui_vidro_fume'),
    (RAIZ + '/ui/kit_emblema_v001.png', 'ui_emblema'),
]
saida = []

for caminho, nome in ENTRADAS:
    if not os.path.exists(caminho):
        saida.append('sem arquivo: ' + os.path.basename(caminho))
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

with open(unreal.Paths.project_saved_dir() + 'TexturasFaltantes.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
