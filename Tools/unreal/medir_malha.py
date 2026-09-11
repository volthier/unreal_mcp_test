# -*- coding: utf-8 -*-
'''Mede a CAIXA DA MALHA importada (nao do ator): isola o problema de orientacao no import.'''

import unreal

saida = []
for caminho in ('/Game/AI_Assets/prop/predio_alto.predio_alto',
                '/Game/AI_Assets/prop/predio_baixo.predio_baixo',
                '/Game/AI_Assets/prop/casulo.casulo',
                '/Game/AI_Assets/prop/cristal_cryonix.cristal_cryonix',
                '/Game/AI_Assets/prop/cristal_nucleo.cristal_nucleo'):
    malha = unreal.EditorAssetLibrary.load_asset(caminho)
    if not malha:
        saida.append('nao carregou: ' + caminho)
        continue
    try:
        caixa = malha.get_bounds()
        extensao = caixa.box_extent
        saida.append('%s: %.2f x %.2f x %.2f m (cento em %.2f, %.2f, %.2f)' % (
            malha.get_name(), extensao.x * 2 / 100, extensao.y * 2 / 100, extensao.z * 2 / 100,
            caixa.origin.x / 100, caixa.origin.y / 100, caixa.origin.z / 100))
    except Exception as erro:
        saida.append(malha.get_name() + ': sem bounds -> ' + str(erro))

with open(unreal.Paths.project_saved_dir() + 'MedirMalha.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
