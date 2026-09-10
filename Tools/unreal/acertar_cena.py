# -*- coding: utf-8 -*-
'''Acerta a cena: lista tudo, poe o PlayerStart no chao (o Landscape esta em z=0) e o cristal tambem.'''

import unreal

saida = []


def registrar(t):
    saida.append(str(t))


unreal.EditorLevelLibrary.load_level('/Game/NewMap')
atores = unreal.EditorLevelLibrary.get_all_level_actors()
registrar('atores no mapa: ' + str(len(atores)))
for ator in atores:
    try:
        onde = ator.get_actor_location()
        registrar('  ' + ator.get_class().get_name() + ' | ' + ator.get_actor_label() + ' | z=' + str(round(onde.z, 1)))
    except Exception:
        registrar('  ' + ator.get_class().get_name() + ' | ' + ator.get_actor_label())

# 1. o chao deste mapa e o Landscape, em z=0. Poe o PlayerStart sobre ele.
for ator in atores:
    if isinstance(ator, unreal.PlayerStart):
        onde = ator.get_actor_location()
        ator.set_actor_location(unreal.Vector(onde.x, onde.y, 150.0), False, False)
        registrar('PlayerStart movido para z=150 (estava em z=' + str(round(onde.z, 1)) + ')')

# 2. o cristal fica no chao, na frente do spawn
for ator in atores:
    if 'cristal' in ator.get_actor_label().lower():
        onde = ator.get_actor_location()
        ator.set_actor_location(unreal.Vector(onde.x, onde.y, 35.0), False, False)
        registrar('cristal assentado em z=35 (estava em z=' + str(round(onde.z, 1)) + ')')

registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'AcertarCena.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
