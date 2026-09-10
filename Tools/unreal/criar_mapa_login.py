# -*- coding: utf-8 -*-
'''Cria o mapa de login como o canonico tinha: /Game/Maps/MainMenuMap.

O canonico (Content/Maps/MainMenuMap.umap) era o GameDefaultMap E o EditorStartupMap, com um segundo mapa
para depois do login (LogedInSelectionMap). Este script cria o de login com a cena que o canonico descreve
para Kardys: praca restaurada, parede limpa cortando parede antiga, colonata, placas burocraticas
(KARDYSHEV, PROTOCOLO ZERO), crepusculo e AR LIMPO - sem nevoa. E com uma CameraActor, para o menu ter
cena de verdade atras da UI em vez de mundo vazio.

Escreve relatorio em Saved/MapaLogin.txt.
'''

import unreal

MAPA = '/Game/Maps/MainMenuMap'
saida = []


def registrar(t):
    saida.append(str(t))


def carregar(caminho):
    return unreal.EditorAssetLibrary.load_asset(caminho) if unreal.EditorAssetLibrary.does_asset_exist(caminho) else None


def por_malha(malha, onde, escala, rotacao, nome):
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, onde, rotacao)
    componente = ator.static_mesh_component
    componente.set_static_mesh(malha)
    componente.set_mobility(unreal.ComponentMobility.MOVABLE)
    ator.set_actor_scale3d(unreal.Vector(escala[0], escala[1], escala[2]) if isinstance(escala, tuple) else unreal.Vector(escala, escala, escala))
    ator.set_actor_label(nome)
    return ator


def por_texto(texto, onde, tamanho, rotacao, nome):
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.TextRenderActor, onde, rotacao)
    ator.text_render.set_text(texto)
    ator.text_render.set_world_size(tamanho)
    ator.set_actor_label(nome)
    return ator


def por_luz(classe, onde, rotacao, nome):
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(classe, onde, rotacao)
    ator.set_actor_label(nome)
    return ator


if unreal.EditorAssetLibrary.does_asset_exist(MAPA):
    registrar('mapa ja existia, abrindo para refazer a cena')
    unreal.EditorLevelLibrary.load_level(MAPA)
else:
    criado = unreal.EditorLevelLibrary.new_level(MAPA)
    registrar('mapa criado: ' + str(criado))

plano = carregar('/Engine/BasicShapes/Plane.Plane')
cubo = carregar('/Engine/BasicShapes/Cube.Cube')
cilindro = carregar('/Engine/BasicShapes/Cylinder.Cylinder')

# ---------- ceu e luz de crepusculo (Kardys: limpa) ----------
por_luz(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0), 'LOGIN_Ceu')
por_luz(unreal.VolumetricCloud, unreal.Vector(0, 0, 3000), unreal.Rotator(0, 0, 0), 'LOGIN_Nuvens')
sol = por_luz(unreal.DirectionalLight, unreal.Vector(0, 0, 2000), unreal.Rotator(-11.0, 200.0, 0.0), 'LOGIN_Sol')
try:
    sol.get_component_by_class(unreal.DirectionalLightComponent).set_intensity(7.0)
    sol.get_component_by_class(unreal.DirectionalLightComponent).set_light_color(unreal.LinearColor(1.0, 0.60, 0.34, 1.0))
    registrar('sol de crepusculo configurado')
except Exception as erro:
    registrar('sol: ' + str(erro))
por_luz(unreal.SkyLight, unreal.Vector(0, 0, 600), unreal.Rotator(0, 0, 0), 'LOGIN_LuzCeu')

# ---------- a praca restaurada ----------
por_malha(plano, unreal.Vector(0, 0, 0), (60.0, 60.0, 1.0), unreal.Rotator(0, 0, 0), 'LOGIN_Piso')

# a parede RESTAURADA (limpa, moderna) cortando a parede ANTIGA (atras, mais alta e torta)
por_malha(cubo, unreal.Vector(0, 2600, 300), (40.0, 0.6, 6.0), unreal.Rotator(0, 0, 0), 'LOGIN_ParedeRestaurada')
por_malha(cubo, unreal.Vector(-900, 3100, 430), (26.0, 1.0, 9.0), unreal.Rotator(0, 7.0, 0), 'LOGIN_ParedeAntiga_A')
por_malha(cubo, unreal.Vector(1200, 3200, 380), (22.0, 1.0, 8.0), unreal.Rotator(0, -6.0, 0), 'LOGIN_ParedeAntiga_B')

# colonata: a restauracao em fileira, como peca de reposicao
for i in range(6):
    x = -1500.0 + i * 600.0
    por_malha(cilindro, unreal.Vector(x, 1900.0, 600.0), (1.6, 1.6, 12.0), unreal.Rotator(0, 0, 0), 'LOGIN_Coluna_%02d' % (i + 1))
    por_malha(cubo, unreal.Vector(x, 1900.0, 1230.0), (1.4, 1.4, 0.6), unreal.Rotator(0, 0, 0), 'LOGIN_Capiteu_%02d' % (i + 1))
registrar('colonata de 6 colunas e as duas paredes montadas')

# ---------- sinalizacao burocratica (canonico: sem destaque) ----------
por_texto('KARDYSHEV', unreal.Vector(0, 2100, 1500), 90.0, unreal.Rotator(0, 90.0, 0), 'LOGIN_Letreiro_Kardyshev')
por_texto('RESTAURO KARDYSHEV  -  SETOR 04  -  PROTOCOLO ZERO', unreal.Vector(0, 2550, 620), 34.0,
          unreal.Rotator(0, 90.0, 0), 'LOGIN_Placa_Setor')
por_texto('RELATORIO DE RESTAURO N. 0117  -  ACESSO CONTROLADO', unreal.Vector(0, 2550, 540), 22.0,
          unreal.Rotator(0, 90.0, 0), 'LOGIN_Placa_Relatorio')

# luz de apoio fria na fachada (o contraste quente/frio do canon)
fria = por_luz(unreal.PointLight, unreal.Vector(-600, 2300, 900), unreal.Rotator(0, 0, 0), 'LOGIN_LuzFria_A')
try:
    fria.get_component_by_class(unreal.PointLightComponent).set_light_color(unreal.LinearColor(0.25, 0.6, 1.0, 1.0))
    fria.get_component_by_class(unreal.PointLightComponent).set_intensity(3000.0)
except Exception as erro:
    registrar('luz fria: ' + str(erro))

# ---------- a camera do menu (o que o jogador ve atras da UI) ----------
camera = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.CameraActor, unreal.Vector(0, -1200, 900), unreal.Rotator(-6.0, 90.0, 0.0))
camera.set_actor_label('LOGIN_Camera')
try:
    camera.camera_component.set_field_of_view(50.0)
    registrar('camera do menu posicionada com FOV 50')
except Exception as erro:
    registrar('camera: ' + str(erro))

registrar('atores no mapa: ' + str(len(unreal.EditorLevelLibrary.get_all_level_actors())))
registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'MapaLogin.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
