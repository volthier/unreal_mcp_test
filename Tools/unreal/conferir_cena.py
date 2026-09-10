# -*- coding: utf-8 -*-
'''Relata o que existe no mapa: chao (Landscape), PlayerStart, o cristal e a distancia entre eles.'''

import unreal

saida = []


def registrar(t):
    saida.append(str(t))


unreal.EditorLevelLibrary.load_level('/Game/NewMap')
atores = unreal.EditorLevelLibrary.get_all_level_actors()
registrar('atores no mapa: ' + str(len(atores)))

for ator in atores:
    tipo = ator.get_class().get_name()
    if tipo in ('Landscape', 'LandscapeStreamingProxy', 'PlayerStart') or 'cristal' in ator.get_actor_label().lower():
        try:
            onde = ator.get_actor_location()
        except Exception:
            onde = 'sem local'
        linha = tipo + ' | ' + ator.get_actor_label() + ' | ' + str(onde)
        try:
            origem, extensao = ator.get_actor_bounds(False)
            linha += ' | centro z=' + str(round(origem.z, 1)) + ' topo z=' + str(round(origem.z + extensao.z, 1))
        except Exception:
            pass
        registrar(linha)

with open(unreal.Paths.project_saved_dir() + 'ConferirCena.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
