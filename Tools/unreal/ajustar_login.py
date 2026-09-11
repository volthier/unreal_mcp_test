# -*- coding: utf-8 -*-
'''Refaz a praca do mapa de login com escala, enquadramento e MATERIAIS da paleta.

A primeira versao saiu errada e a captura provou: praca de 60 m com colunas de 12 m vistas de 19 m de
distancia e camera quase horizontal = descampado com palitos no horizonte. E tudo em cinza de blockout.

Agora: colonata perto da camera, pecas com escala monumental, materiais de latao/pedra/aco, letreiro
KARDYSHEV na parede e enquadramento em tres-quartos para o fundo do menu ter cena de verdade.

Idempotente: apaga os atores LOGIN_* antes de refazer. Relatorio em Saved/AjustarLogin.txt.
'''

import unreal

MAPA = '/Game/Maps/MainMenuMap'
PASTA_MAT = '/Game/AI_Assets/materials'
saida = []


def registrar(t):
    saida.append(str(t))


def material(nome, cor, rugosidade, metalico, emissivo=None):
    completo = PASTA_MAT + '/' + nome
    if unreal.EditorAssetLibrary.does_asset_exist(completo):
        return unreal.EditorAssetLibrary.load_asset(completo)
    ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
    mat = ferramentas.create_asset(nome, PASTA_MAT, unreal.Material, unreal.MaterialFactoryNew())
    if mat is None:
        return None
    base = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector, -500, -200)
    base.set_editor_property('constant', cor)
    unreal.MaterialEditingLibrary.connect_material_property(base, '', unreal.MaterialProperty.MP_BASE_COLOR)
    rug = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant, -500, 250)
    rug.set_editor_property('r', rugosidade)
    unreal.MaterialEditingLibrary.connect_material_property(rug, '', unreal.MaterialProperty.MP_ROUGHNESS)
    met = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant, -500, 450)
    met.set_editor_property('r', metalico)
    unreal.MaterialEditingLibrary.connect_material_property(met, '', unreal.MaterialProperty.MP_METALLIC)
    if emissivo is not None:
        emi = unreal.MaterialEditingLibrary.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector, -500, 650)
        emi.set_editor_property('constant', emissivo)
        unreal.MaterialEditingLibrary.connect_material_property(emi, '', unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    unreal.MaterialEditingLibrary.recompile_material(mat)
    unreal.EditorAssetLibrary.save_asset(completo)
    return mat


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


def por_texto(texto, onde, tamanho, rotacao, nome):
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.TextRenderActor, onde, rotacao)
    ator.text_render.set_text(texto)
    ator.text_render.set_world_size(tamanho)
    ator.set_actor_label(nome)
    return ator


unreal.EditorLevelLibrary.load_level(MAPA)

for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label().startswith('LOGIN_'):
        unreal.EditorLevelLibrary.destroy_actor(ator)
registrar('praca anterior limpa')

m_latao = material('M_LataoPolido', unreal.LinearColor(0.62, 0.42, 0.12, 1.0), 0.28, 0.9)
m_pedra = material('M_PedraRestaurada', unreal.LinearColor(0.30, 0.29, 0.27, 1.0), 0.75, 0.0)
m_antiga = material('M_PedraAntiga', unreal.LinearColor(0.16, 0.15, 0.14, 1.0), 0.9, 0.0)
m_aco = material('M_AcoEscuro', unreal.LinearColor(0.07, 0.07, 0.08, 1.0), 0.35, 0.7)
m_neon = material('M_NeonFrio', unreal.LinearColor(0.2, 0.7, 1.0, 1.0), 0.4, 0.0, emissivo=unreal.LinearColor(0.15, 0.9, 1.6, 1.0))
registrar('materiais: latao, pedra restaurada, pedra antiga, aco, neon')

plano = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Plane.Plane')
cubo = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Cube.Cube')
cilindro = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Cylinder.Cylinder')

# ceu e sol
unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyAtmosphere, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0)).set_actor_label('LOGIN_Ceu')
unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.VolumetricCloud, unreal.Vector(0, 0, 4000), unreal.Rotator(0, 0, 0)).set_actor_label('LOGIN_Nuvens')
sol = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.DirectionalLight, unreal.Vector(0, 0, 3000), unreal.Rotator(-8.0, 195.0, 0.0))
sol.set_actor_label('LOGIN_Sol')
try:
    sol.get_component_by_class(unreal.DirectionalLightComponent).set_intensity(8.0)
    sol.get_component_by_class(unreal.DirectionalLightComponent).set_light_color(unreal.LinearColor(1.0, 0.58, 0.32, 1.0))
except Exception as erro:
    registrar('sol: ' + str(erro))
unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SkyLight, unreal.Vector(0, 0, 800), unreal.Rotator(0, 0, 0)).set_actor_label('LOGIN_LuzCeu')

# piso da praca: 30 m, nao 60
por_malha(plano, unreal.Vector(0, 300, 0), (60.0, 60.0, 1.0), unreal.Rotator(0, 0, 0), 'LOGIN_Piso', m_aco)

# parede RESTAURADA em primeiro plano (a limpa, cortando) e a ANTIGA atras, mais alta e torta
por_malha(cubo, unreal.Vector(0, 1500, 500), (16.0, 0.3, 10.0), unreal.Rotator(0, 0, 0), 'LOGIN_ParedeRestaurada', m_pedra)
por_malha(cubo, unreal.Vector(-1600, 2800, 450), (10.0, 0.6, 9.0), unreal.Rotator(0, 6.0, 0), 'LOGIN_ParedeAntiga_A', m_antiga)
por_malha(cubo, unreal.Vector(1500, 2900, 400), (9.0, 0.6, 8.0), unreal.Rotator(0, -5.0, 0), 'LOGIN_ParedeAntiga_B', m_antiga)

# colonata: pecas MONUMENTAIS, proximas, em pares - 14 m de altura, 1,4 m de raio
for i in range(5):
    x = -1200.0 + i * 600.0
    por_malha(cilindro, unreal.Vector(x, 900.0, 700.0), (1.4, 1.4, 14.0), unreal.Rotator(0, 0, 0), 'LOGIN_Coluna_%02d' % (i + 1), m_latao)
    por_malha(cubo, unreal.Vector(x, 900.0, 1430.0), (1.2, 1.2, 0.5), unreal.Rotator(0, 0, 0), 'LOGIN_Capiteu_%02d' % (i + 1), m_latao)
    por_malha(cubo, unreal.Vector(x, 900.0, 30.0), (1.3, 1.3, 0.35), unreal.Rotator(0, 0, 0), 'LOGIN_Base_%02d' % (i + 1), m_latao)
registrar('colonata monumental de 5 pecas montada em y=900')

# fita de neon frio na base da parede restaurada (o contraste quente/frio do canon)
por_malha(cubo, unreal.Vector(0, 1450, 60.0), (15.0, 0.08, 0.12), unreal.Rotator(0, 0, 0), 'LOGIN_Neon', m_neon)

# letreiro na parede restaurada, na altura dos olhos de quem entra
por_texto('KARDYSHEV', unreal.Vector(0, 1430, 1000), 130.0, unreal.Rotator(0, 90.0, 0), 'LOGIN_Letreiro')
por_texto('RESTAURO  -  SETOR 04  -  PROTOCOLO ZERO', unreal.Vector(0, 1430, 820), 34.0, unreal.Rotator(0, 90.0, 0), 'LOGIN_Placa_Setor')
por_texto('RELATORIO DE RESTAURO N. 0117', unreal.Vector(0, 1430, 740), 24.0, unreal.Rotator(0, 90.0, 0), 'LOGIN_Placa_Relatorio')

# luz de apoio: quente de baixo (chao) e fria na fachada
quente = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PointLight, unreal.Vector(0, 700, 250), unreal.Rotator(0, 0, 0))
quente.set_actor_label('LOGIN_LuzQuente')
try:
    quente.get_component_by_class(unreal.PointLightComponent).set_light_color(unreal.LinearColor(1.0, 0.65, 0.3, 1.0))
    quente.get_component_by_class(unreal.PointLightComponent).set_intensity(6000.0)
    quente.get_component_by_class(unreal.PointLightComponent).set_attenuation_radius(2500.0)
except Exception as erro:
    registrar('luz quente: ' + str(erro))

# camera: tres-quartos, 7 m de altura, olhando a colonata de perto (enquadramento de fundo de menu)
camera = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.CameraActor, unreal.Vector(-900, -1500, 950), unreal.Rotator(-12.0, 72.0, 0.0))
camera.tags = ['MenuCamera']
camera.set_actor_label('LOGIN_Camera')
try:
    camera.camera_component.set_field_of_view(50.0)
except Exception as erro:
    registrar('camera: ' + str(erro))
registrar('camera em tres-quartos marcada com a tag MenuCamera')

registrar('atores no mapa: ' + str(len(unreal.EditorLevelLibrary.get_all_level_actors())))
registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'AjustarLogin.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
