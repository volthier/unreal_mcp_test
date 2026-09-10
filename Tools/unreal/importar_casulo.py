# -*- coding: utf-8 -*-
'''Importa o casulo gerado e substitui os cilindros provisorios da fileira.'''

import unreal

FBX = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/3d/casulo_game.fbx'
PASTA = '/Game/AI_Assets/prop'
MAPA = '/Game/NewMap'

saida = []


def registrar(t):
    saida.append(str(t))


tarefa = unreal.AssetImportTask()
tarefa.filename = FBX
tarefa.destination_path = PASTA
tarefa.destination_name = 'casulo'
tarefa.automated = True
tarefa.replace_existing = True
tarefa.save = True
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
registrar('importado: ' + str(tarefa.imported_object_paths))

malha = unreal.EditorAssetLibrary.load_asset(PASTA + '/casulo.casulo')
registrar('malha carregada: ' + str(malha is not None))

unreal.EditorLevelLibrary.load_level(MAPA)

for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    rotulo = ator.get_actor_label()
    if rotulo.startswith('CENA_CASULO_') and malha is not None:
        ator.static_mesh_component.set_static_mesh(malha)
        ator.set_actor_scale3d(unreal.Vector(1.0, 1.0, 1.0))
        registrar('casulo aplicado em: ' + rotulo)

registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'ImportarCasulo.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
