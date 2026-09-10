# -*- coding: utf-8 -*-
'''Declara o GameMode do menu dentro do proprio mapa (WorldSettings), como o NewMap faz.

Por que: o projeto tem GlobalDefaultGameMode, mas mapa de menu deve se declarar - e o teste
Runner.Entrada.MapaApontaParaOMenu le exatamente essa propriedade no WorldSettings.
'''

import unreal

MAPA = '/Game/Maps/MainMenuMap'
GAMEMODE = '/Script/PloidrekRPG.MenuGameMode'
saida = []


def registrar(t):
    saida.append(str(t))


unreal.EditorLevelLibrary.load_level(MAPA)
mundo = unreal.EditorLevelLibrary.get_editor_world()
config = mundo.get_world_settings()
classe = unreal.load_class(None, GAMEMODE)
registrar('classe do game mode: ' + str(classe))
if classe:
    config.set_editor_property('default_game_mode', classe)
    registrar('WorldSettings do mapa agora aponta para MenuGameMode')
registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'GameModeMapa.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
