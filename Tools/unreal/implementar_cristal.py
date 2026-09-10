# -*- coding: utf-8 -*-
'''Conserta o cristal no mapa: reimporta a malha, cria material emissivo e POE NO CHAO.

Corrige dois defeitos do primeiro import: o ator ficou flutuando (o PlayerStart do mapa esta a 13 m
de altura, e eu coloquei o objeto relativo a ele) e a malha veio preta (normais invertidas e material
padrao do FBX). Aqui o objeto e posto SOBRE o chao, com escala de objeto de mesa e material emissivo
na cor do nucleo do chassi.

Escreve o relatorio em Saved/ImplementarCristal.txt (unreal.log() nao chega ao log em commandlet).
'''

import unreal

NOME = 'cristal_cryonix'
FBX = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/3d/cristal_cryonix_game.fbx'
PASTA = '/Game/AI_Assets'
MAPA = '/Game/NewMap'
COR = unreal.LinearColor(0.35, 0.75, 1.0, 1.0)
BRILHO = unreal.LinearColor(1.2, 4.0, 6.0, 1.0)
ESCALA = 0.35

saida = []


def registrar(t):
    saida.append(str(t))


def criar_material():
    caminho = PASTA + '/materials'
    completo = caminho + '/M_CristalNucleo'
    if unreal.EditorAssetLibrary.does_asset_exist(completo):
        return unreal.EditorAssetLibrary.load_asset(completo)
    ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
    material = ferramentas.create_asset('M_CristalNucleo', caminho, unreal.Material, unreal.MaterialFactoryNew())
    if material is None:
        return None
    base = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionConstant3Vector, -500, -150)
    base.set_editor_property('constant', COR)
    unreal.MaterialEditingLibrary.connect_material_property(base, '', unreal.MaterialProperty.MP_BASE_COLOR)
    emissivo = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionConstant3Vector, -500, 150)
    emissivo.set_editor_property('constant', BRILHO)
    unreal.MaterialEditingLibrary.connect_material_property(emissivo, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    rugosidade = unreal.MaterialEditingLibrary.create_material_expression(material, unreal.MaterialExpressionConstant, -500, 420)
    rugosidade.set_editor_property('r', 0.18)
    unreal.MaterialEditingLibrary.connect_material_property(rugosidade, '', unreal.MaterialProperty.MP_ROUGHNESS)
    unreal.MaterialEditingLibrary.recompile_material(material)
    unreal.EditorAssetLibrary.save_asset(completo)
    return material


tarefa = unreal.AssetImportTask()
tarefa.filename = FBX
tarefa.destination_path = PASTA + '/prop'
tarefa.destination_name = NOME
tarefa.automated = True
tarefa.replace_existing = True
tarefa.save = True
unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([tarefa])
registrar('reimportado: ' + str(tarefa.imported_object_paths))

malha = unreal.EditorAssetLibrary.load_asset(PASTA + '/prop/' + NOME + '.' + NOME)
material = criar_material()
registrar('material: ' + str(material))

unreal.EditorLevelLibrary.load_level(MAPA)

for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label().startswith(NOME):
        registrar('ator antigo removido: ' + ator.get_actor_label())
        unreal.EditorLevelLibrary.destroy_actor(ator)

partidas = [a for a in unreal.EditorLevelLibrary.get_all_level_actors() if isinstance(a, unreal.PlayerStart)]
origem = partidas[0].get_actor_location() if partidas else unreal.Vector(0, 0, 0)
registrar('PlayerStart em ' + str(origem))

# O PlayerStart fica NO CHAO por convencao - e a referencia de solo. Nao usar line_trace_single:
# nesta versao do UE ele devolve None e derruba o script (foi o que aconteceu na primeira tentativa).
onde_x = origem.x + 300.0
ponto = unreal.Vector(onde_x, origem.y, 0.0)
registrar('solo do mapa (Landscape em z=0): ' + str(ponto))

onde = unreal.Vector(ponto.x, ponto.y, 35.0)
cristal = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, onde, unreal.Rotator(0, 0, 0))
componente = cristal.static_mesh_component
componente.set_static_mesh(malha)
componente.set_material(0, material)
componente.set_mobility(unreal.ComponentMobility.MOVABLE)
cristal.set_actor_scale3d(unreal.Vector(ESCALA, ESCALA, ESCALA))
cristal.set_actor_label(NOME + '_no_chao')
registrar('cristal em ' + str(onde) + ' escala ' + str(ESCALA))
registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    pass
registrar('total de atores visiveis nesta sessao: ' + str(len(unreal.EditorLevelLibrary.get_all_level_actors())))

with open(unreal.Paths.project_saved_dir() + 'ImplementarCristal.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
