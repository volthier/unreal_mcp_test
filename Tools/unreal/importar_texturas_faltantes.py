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
    # NEVOA DE GELO SECO (referencia do autor, foto de gelo seco): a PLUMA com volutas entra no lugar da
    # textura antiga, com o MESMO nome - e o mesmo parametro TexturaDaAura do material da aura, entao a arte
    # nova passa a valer sem mexer no material. A POCA e textura nova, para a nevoa que se acumula na base.
    (RAIZ + '/fx/fumaca_seca_v001.png', 'fx_nevoa_aura'),
    (RAIZ + '/fx/fumaca_seca_poca_v001.png', 'fx_nevoa_poca'),
    (RAIZ + '/cenario/login_aether_escolhido.png', 'ui_nuvens_ocre'),   # AETHER FORGE: tempestade de nano-robos
    (RAIZ + '/cenario/login_kardys_v001.png', 'ui_login_kardys'),
    (RAIZ + '/cenario/pods_v002.png', 'ui_pods_selecao'),
    # Kit visual AAA do menu (EXC-005): o TextureTools.import_file do MCP recusa estes PNGs
    # PAINEL DE HUD do login (alvo do autor): canto arredondado, borda ciano fina e COLCHETES de canto, no
    # lugar da moldura de latao com interior branco. Mesmo nome de asset, entao a config nao muda.
    # FRAME hard surface do painel (spec secao 3): cantos cortados em 45 graus, chanfros, trim ciano emissivo,
    # accents magenta, linhas de painel e parafusos. Mesmo nome de asset, entao a config nao muda.
    (RAIZ + '/ui/frame_login_v001.png', 'ui_moldura_latao'),
    # Kit do login: o logotipo com o X em degrade e os icones dos campos.
    (RAIZ + '/ui/logo_aether.png', 'ui_logo_aether'),   # refeito na Orbitron do guia
    (RAIZ + '/ui/icone_pessoa.png', 'ui_icone_pessoa'),
    (RAIZ + '/ui/icone_cadeado.png', 'ui_icone_cadeado'),
    (RAIZ + '/ui/icone_olho.png', 'ui_icone_olho'),
    (RAIZ + '/ui/icone_olho_fechado.png', 'ui_icone_olho_fechado'),
    # Simbolos PROPRIOS dos botoes sociais: geometria neutra desenhada por nos, sem marca de terceiros (o autor
    # foi explicito: nada de relacao com nome ou asset de outro).
    (RAIZ + '/ui/icone_rede1.png', 'ui_rede1'), (RAIZ + '/ui/icone_rede2.png', 'ui_rede2'),
    (RAIZ + '/ui/icone_rede3.png', 'ui_rede3'), (RAIZ + '/ui/icone_rede4.png', 'ui_rede4'),
    (RAIZ + '/ui/icone_rede5.png', 'ui_rede5'), (RAIZ + '/ui/icone_rede6.png', 'ui_rede6'),
    (RAIZ + '/ui/icone_rede7.png', 'ui_rede7'),
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
