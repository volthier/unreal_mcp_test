# -*- coding: utf-8 -*-
'''Deixa o mapa de login VAZIO (como o autor pediu): a praca sai, a arte carrega o visual.'''

import unreal

MAPA = '/Game/Maps/MainMenuMap'
saida = []

unreal.EditorLevelLibrary.load_level(MAPA)
removidos = 0
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    rotulo = ator.get_actor_label()
    if rotulo.startswith('LOGIN_') and rotulo != 'LOGIN_Camera':
        unreal.EditorLevelLibrary.destroy_actor(ator)
        removidos += 1
saida.append('atores da praca removidos: ' + str(removidos))
restantes = [a.get_actor_label() for a in unreal.EditorLevelLibrary.get_all_level_actors()]
saida.append('atores agora: ' + ', '.join(restantes[:12]))
saida.append('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'EsvaziarLogin.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
