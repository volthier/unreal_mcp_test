# -*- coding: utf-8 -*-
'''Monta a cena inicial seguindo o canonico (Docs/WorldBible_Nexus7.md 6b).

O canonico diz, e isto esta sendo seguido aqui:
  - CASULOS EM FILEIRA: alinhados e NUMERADOS, com linguagem visual de peca de reposicao;
  - CIDADE LIMPA: sem Veu, sem nevoa, sem fumaca - o unico lugar onde o horizonte nao mente;
  - DUAS CAMADAS DE NOME: sinalizacao oficial escreve KARDYSHEV, a rua diz Kardys;
  - 'PROTOCOLO ZERO' por todo lado SEM destaque, com cara de burocracia.

Escreve relatorio em Saved/MontarCena.txt (unreal.log nao chega ao log em commandlet).
'''

import unreal

MAPA = '/Game/NewMap'
PASTA_CASULO = '/Game/AI_Assets/prop/casulo'
PASTA_PROP = '/Game/AI_Assets/prop'

saida = []


def registrar(t):
    saida.append(str(t))


def carregar(caminho):
    return unreal.EditorAssetLibrary.load_asset(caminho) if unreal.EditorAssetLibrary.does_asset_exist(caminho) else None


def criar_malha(malha, onde, escala, rotacao, nome):
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, onde, rotacao)
    componente = ator.static_mesh_component
    componente.set_static_mesh(malha)
    componente.set_mobility(unreal.ComponentMobility.MOVABLE)
    ator.set_actor_scale3d(unreal.Vector(escala, escala, escala))
    ator.set_actor_label(nome)
    return ator


def criar_texto(texto, onde, tamanho, rotacao, nome):
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.TextRenderActor, onde, rotacao)
    componente = ator.text_render
    componente.set_text(texto)
    componente.set_world_size(tamanho)
    # o nome do enum de alinhamento muda entre versoes da engine: cosmetico, nao pode derrubar a cena
    for enums_nome, valor in (('HorizTextAligment', 'EHTA_CENTER'), ('VerticalTextAligment', 'EVTA_CENTER')):
        try:
            enums = getattr(unreal, enums_nome, None)
            if enums is not None and hasattr(enums, valor):
                componente.set_editor_property('horizontal_alignment' if enums_nome.startswith('Horiz') else 'vertical_alignment',
                                               getattr(enums, valor))
        except Exception:
            pass
    ator.set_actor_label(nome)
    return ator


unreal.EditorLevelLibrary.load_level(MAPA)
mundo = unreal.EditorLevelLibrary.get_editor_world()

# limpa o que EU criei antes (nao mexe no que o autor pos)
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    rotulo = ator.get_actor_label()
    if rotulo.startswith('cristal_cryonix') or rotulo.startswith('CENA_'):
        unreal.EditorLevelLibrary.destroy_actor(ator)
        registrar('removido: ' + rotulo)

cilindro = carregar('/Engine/BasicShapes/Cylinder.Cylinder')
cubo = carregar('/Engine/BasicShapes/Cube.Cube')
casulo = carregar(PASTA_CASULO + '/casulo.casulo')
registrar('malha do casulo importada: ' + str(casulo is not None))

# ---------- 1. a fileira de casulos (canonico: alinhados e numerados) ----------
y_fileira = 700.0
for indice in range(6):
    x = -1600.0 + indice * 420.0
    base = criar_malha(cilindro, unreal.Vector(x, y_fileira, 15.0), 1.6, unreal.Rotator(0, 0, 0), 'CENA_CASULO_BASE_%02d' % (indice + 1))
    if casulo is not None:
        criar_malha(casulo, unreal.Vector(x, y_fileira, 190.0), 1.0, unreal.Rotator(0, 0, 0), 'CENA_CASULO_%02d' % (indice + 1))
    else:
        criar_malha(cilindro, unreal.Vector(x, y_fileira, 190.0), 1.3, unreal.Rotator(0, 0, 0), 'CENA_CASULO_%02d' % (indice + 1))
    criar_texto('CASULO %02d / RECARGA' % (indice + 1), unreal.Vector(x, y_fileira - 120.0, 55.0), 26.0,
                unreal.Rotator(0, 0, 90.0), 'CENA_PLACA_%02d' % (indice + 1))
registrar('fileira de 6 casulos montada em y=%.0f' % y_fileira)

# ---------- 2. o cristal SOBRE um pedestal, de frente para o spawn ----------
x_cristal, y_cristal = -800.0, 150.0
criar_malha(cilindro, unreal.Vector(x_cristal, y_cristal, 55.0), 1.1, unreal.Rotator(0, 0, 0), 'CENA_PEDESTAL')
criar_malha(cilindro, unreal.Vector(x_cristal, y_cristal, 112.0), 1.25, unreal.Rotator(0, 0, 0), 'CENA_PEDESTAL_TOPO')
criar_texto('KARDYSHEV / NUCLEO CRYONIX', unreal.Vector(x_cristal, y_cristal - 150.0, 40.0), 22.0,
            unreal.Rotator(0, 0, 90.0), 'CENA_PLACA_NUCLEO')

malha_cristal = carregar(PASTA_PROP + '/cristal_cryonix.cristal_cryonix')
if malha_cristal:
    cristal = criar_malha(malha_cristal, unreal.Vector(x_cristal, y_cristal, 112.0 + 70.0), 0.35,
                          unreal.Rotator(0, 0, 0), 'cristal_cryonix_no_pedestal')
    material = carregar(PASTA_PROP.replace('/prop', '/materials') + '/M_CristalNucleo.M_CristalNucleo')
    if material:
        cristal.static_mesh_component.set_material(0, material)
        registrar('material do cristal reaplicado')
    else:
        registrar('ATENCAO: material M_CristalNucleo nao encontrado')
else:
    registrar('ATENCAO: malha do cristal nao encontrada')

# luz de vitrine: um foco de cima + um ponto quente junto do cristal
foco = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.SpotLight, unreal.Vector(x_cristal, y_cristal - 260.0, 420.0),
                                                          unreal.Rotator(-38.0, 90.0, 0.0))
foco.spot_light_component.set_intensity(9000.0)
foco.spot_light_component.set_light_color(unreal.LinearColor(1.0, 0.72, 0.42, 1.0))
foco.spot_light_component.set_outer_cone_angle(34.0)
foco.set_actor_label('CENA_LUZ_VITRINE')

ponto = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PointLight, unreal.Vector(x_cristal, y_cristal, 200.0), unreal.Rotator(0, 0, 0))
ponto.point_light_component.set_intensity(2600.0)
ponto.point_light_component.set_light_color(unreal.LinearColor(0.35, 0.75, 1.0, 1.0))
ponto.point_light_component.set_attenuation_radius(700.0)
ponto.set_actor_label('CENA_LUZ_NUCLEO')
registrar('vitrine montada em x=%.0f y=%.0f' % (x_cristal, y_cristal))

# ---------- 3. o spawn: de frente para a fileira e para a vitrine ----------
partidas = [a for a in unreal.EditorLevelLibrary.get_all_level_actors() if isinstance(a, unreal.PlayerStart)]
if partidas:
    partidas[0].modify()
    partidas[0].set_actor_location(unreal.Vector(-800.0, -900.0, 150.0), False, False)
    partidas[0].set_actor_rotation(unreal.Rotator(0, 90.0, 0), False)
    registrar('PlayerStart posicionado de frente para o conjunto')

# ---------- 4. sinalizacao burocratica (canonico: sem destaque) ----------
criar_texto('PROTOCOLO ZERO  -  SETOR 04  -  RESTAURO KARDYSHEV', unreal.Vector(-800.0, 1500.0, 520.0), 34.0,
            unreal.Rotator(0, 90.0, 0), 'CENA_PLACA_SETOR')
criar_texto('ACESSO CONTROLADO  -  RELATORIO DE RESTAURO N. 0117', unreal.Vector(-800.0, 1500.0, 440.0), 22.0,
            unreal.Rotator(0, 90.0, 0), 'CENA_PLACA_RELATORIO')

# ---------- 5. luz de crepusculo e AR LIMPO (o canonico proibe nevoa aqui) ----------
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    nome = ator.get_class().get_name()
    if nome == 'DirectionalLight':
        ator.set_actor_rotation(unreal.Rotator(-9.0, 205.0, 0.0), False)
        try:
            ator.directional_light_component.set_light_color(unreal.LinearColor(1.0, 0.62, 0.36, 1.0))
            ator.directional_light_component.set_intensity(6.0)
        except Exception as erro:
            registrar('luz direcional: ' + str(erro))
        registrar('sol posicionado no crepusculo')
    if nome == 'ExponentialHeightFog':
        try:
            ator.get_component_by_class(unreal.ExponentialHeightFogComponent).set_editor_property('fog_density', 0.0)
            ator.set_actor_hidden_in_game(True)
            registrar('nevoa DESLIGADA (canonico: Kardys e limpa)')
        except Exception as erro:
            registrar('nevoa: ' + str(erro))

registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'MontarCena.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
