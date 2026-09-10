# -*- coding: utf-8 -*-
'''Poe o PlayerStart no chao e SALVA de verdade (marca o pacote sujo e salva o diretorio).'''

import unreal

saida = []


def registrar(t):
    saida.append(str(t))


unreal.EditorLevelLibrary.load_level('/Game/NewMap')

for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if isinstance(ator, unreal.PlayerStart):
        antes = ator.get_actor_location()
        ator.modify()
        ator.set_actor_location(unreal.Vector(antes.x, antes.y, 150.0), False, False)
        registrar('PlayerStart: z ' + str(round(antes.z, 1)) + ' -> ' + str(round(ator.get_actor_location().z, 1)))

registrar('nivel salvo: ' + str(unreal.EditorLevelLibrary.save_current_level()))
registrar('assets salvos: ' + str(unreal.EditorAssetLibrary.save_directory('/Game/NewMap', only_if_is_dirty=False, recursive=True)))

# confere relendo do disco
unreal.EditorLevelLibrary.load_level('/Game/NewMap')
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if isinstance(ator, unreal.PlayerStart):
        registrar('depois de recarregar, PlayerStart z=' + str(round(ator.get_actor_location().z, 1)))

with open(unreal.Paths.project_saved_dir() + 'PlayerStart.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
