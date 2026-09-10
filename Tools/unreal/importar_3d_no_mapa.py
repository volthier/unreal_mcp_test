# -*- coding: utf-8 -*-
"""Importa a malha 3D gerada e POE ELA NO MAPA, para o autor ver em jogo.

Fluxo: importa o FBX como StaticMesh em /Game/AI_Assets/prop/<nome>, cria um ator no mapa do menu
(NewMap) perto do PlayerStart e salva o mapa. Assim, ao dar Play, o objeto esta no mundo.

Escreve um relatorio em Saved/Import3D.txt porque unreal.log() nao chega ao log em commandlet.
"""

import unreal

NOME = 'cristal_cryonix'
ORIGEM = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/3d/cristal_cryonix_game.fbx'
DESTINO = '/Game/AI_Assets/prop'
MAPA = '/Game/NewMap'
DISTANCIA = 450.0

linhas = []


def registrar(texto):
    linhas.append(str(texto))


tarefa = unreal.AssetImportTask()
tarefa.filename = ORIGEM
tarefa.destination_path = DESTINO
tarefa.destination_name = NOME
tarefa.automated = True
tarefa.replace_existing = True
tarefa.save = True
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
registrar('importados: %s' % tarefa.imported_object_paths)

caminho_malha = DESTINO + '/' + NOME + '.' + NOME
malha = unreal.EditorAssetLibrary.load_asset(caminho_malha)
registrar('malha carregada: %s' % malha)
if malha is None:
    unreal.SystemLibrary.quit_editor()

mundo = unreal.EditorLevelLibrary.get_editor_world()
unreal.EditorLevelLibrary.load_level(MAPA)

atores = unreal.EditorLevelLibrary.get_all_level_actors()
partidas = [a for a in atores if isinstance(a, unreal.PlayerStart)]
registrar('PlayerStart encontrados: %d' % len(partidas))

base = partidas[0].get_actor_location() if partidas else unreal.Vector(0, 0, 300)
onde = unreal.Vector(base.x + DISTANCIA, base.y, base.z + 120)
registrar('colocando em %s' % onde)

rotacao = unreal.Rotator(0, 0, 0)
ator = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, onde, rotacao)
componente = ator.static_mesh_component
componente.set_static_mesh(malha)
componente.set_mobility(unreal.ComponentMobility.MOVABLE)
ator.set_actor_label(NOME + '_no_mapa')
registrar('ator criado: %s' % ator.get_actor_label())

salvou = unreal.EditorLoadingAndSavingUtils.save_current_level()
registrar('mapa salvo: %s' % salvou)

total = len(unreal.EditorLevelLibrary.get_all_level_actors())
registrar('atores no mapa agora: %d' % total)

with open(unreal.Paths.project_saved_dir() + 'Import3D.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(linhas) + chr(10))
