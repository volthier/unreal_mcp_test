# -*- coding: utf-8 -*-
'''Cria o mapa do MUNDO ABERTO (a area inicial): praca industrial + ceu de crepusculo + PlayerStart.

O que o canonico pede para Kardys e respeitado: cidade limpa (sem Veu, sem nevoa). O chao e a plataforma
industrial; a CIDADE em volta entra depois, por PCG (procedimental), como o autor pediu.
'''

import unreal

MAPA = '/Game/Maps/MundoAberto'
saida = []


def carregar(caminho):
    return unreal.EditorAssetLibrary.load_asset(caminho) if unreal.EditorAssetLibrary.does_asset_exist(caminho) else None


def por_malha(malha, onde, escala, rotacao, nome, mat=None):
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, onde, rotacao)
    comp = ator.static_mesh_component
    comp.set_static_mesh(malha)
    comp.set_mobility(unreal.ComponentMobility.MOVABLE)
    ator.set_actor_scale3d(unreal.Vector(escala[0], escala[1], escala[2]))
    if mat:
        comp.set_material(0, mat)
    ator.set_actor_label(nome)
    return ator


if unreal.EditorAssetLibrary.does_asset_exist(MAPA):
    unreal.EditorLevelLibrary.load_level(MAPA)
    saida.append('mapa ja existia: ' + MAPA)
else:
    saida.append('mapa criado: ' + str(unreal.EditorLevelLibrary.new_level(MAPA)))

# IDEMPOTENTE: apaga o que ESTE script criou antes de refazer. Sem isso, rodar de novo duplicava o sol e
# o engine passava a avisar 'multiple directional lights are competing' - dois sois no mesmo ceu.
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label().startswith('MUNDO_'):
        unreal.EditorLevelLibrary.destroy_actor(ator)
saida.append('atores MUNDO_* anteriores limpos')

# ceu de crepusculo e luz
for classe, onde, rot, nome in (
    (unreal.SkyAtmosphere, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0), 'MUNDO_Ceu'),
    (unreal.VolumetricCloud, unreal.Vector(0, 0, 5000), unreal.Rotator(0, 0, 0), 'MUNDO_Nuvens'),
    (unreal.SkyLight, unreal.Vector(0, 0, 900), unreal.Rotator(0, 0, 0), 'MUNDO_LuzCeu'),
    (unreal.DirectionalLight, unreal.Vector(0, 0, 4000), unreal.Rotator(roll=0.0, pitch=-10.0, yaw=200.0), 'MUNDO_Sol'),
):
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(classe, onde, rot)
    ator.set_actor_label(nome)
    if nome == 'MUNDO_Sol':
        try:
            ator.get_component_by_class(unreal.DirectionalLightComponent).set_intensity(7.0)
            ator.get_component_by_class(unreal.DirectionalLightComponent).set_light_color(unreal.LinearColor(1.0, 0.6, 0.34, 1.0))
        except Exception as erro:
            saida.append('sol: ' + str(erro))

# a praca industrial: plataforma grande de aco escuro onde a area inicial comeca
plano = carregar('/Engine/BasicShapes/Plane.Plane')
aco = carregar('/Game/AI_Assets/materials/M_AcoEscuro.M_AcoEscuro')
por_malha(plano, unreal.Vector(0, 0, 0), (120.0, 120.0, 1.0), unreal.Rotator(0, 0, 0), 'MUNDO_Piso', aco)

# marcacao de setor no chao (o canonico: 'PROTOCOLO ZERO' como burocracia)
texto = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.TextRenderActor, unreal.Vector(0, -1800, 60), unreal.Rotator(roll=0.0, pitch=0.0, yaw=90.0))
texto.text_render.set_text('PROTOCOLO ZERO  -  SETOR 04  -  KARDYSHEV')
texto.text_render.set_world_size(90.0)
texto.set_actor_label('MUNDO_Placa')

# PlayerStart no meio da praca, olhando para o eixo onde a cidade vai crescer
inicio = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PlayerStart, unreal.Vector(0, -600, 150), unreal.Rotator(roll=0.0, pitch=0.0, yaw=90.0))
inicio.set_actor_label('MUNDO_PlayerStart')

# o mapa se declara: o jogo comeca em AFGameMode (o do mundo, nao o do menu)
mundo = unreal.EditorLevelLibrary.get_editor_world()
config = mundo.get_world_settings()
classe = unreal.load_class(None, '/Script/PloidrekRPG.AFGameMode')
if classe:
    config.set_editor_property('default_game_mode', classe)
    saida.append('WorldSettings aponta para AFGameMode')

saida.append('atores no mapa: ' + str(len(unreal.EditorLevelLibrary.get_all_level_actors())))
saida.append('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'MundoAberto.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
