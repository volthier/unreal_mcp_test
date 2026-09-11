# -*- coding: utf-8 -*-
'''Monta a AREA INICIAL do mundo aberto: a cidade em volta da praca, por procedimento.

Como funciona (e por que assim): o grafo de PCG da cidade existe em /Game/AI_Assets/pcg/PCG_CidadeKardys,
criado pela MCP do Unreal - mas a MCP cria o asset na MEMORIA do editor e o pipeline headless nao ve o que
nao esta em disco. Entao a cidade desta passada e gerada por PROCEDIMENTO aqui (grade + jitter + variacao de
altura e material, com semente fixa para ser reproduzivel), e o PCG assume quando o grafo estiver salvo.

O canonico e respeitado: Kardys e limpa (sem nevoa), vertical, com placas solares e hologramas - dai a
variacao de altura e a paleta de latao/cobre/aco.
'''

import math
import random
import unreal

MAPA = '/Game/Maps/MundoAberto'
SEMENTE = 20260911
LADO = 5              # 5x5 quarteiroes
PASSO = 6500.0        # 65 m entre predios
RECUO = 9000.0        # a praca central fica livre

saida = []
random.seed(SEMENTE)


def carregar(caminho):
    return unreal.EditorAssetLibrary.load_asset(caminho) if unreal.EditorAssetLibrary.does_asset_exist(caminho) else None


unreal.EditorLevelLibrary.load_level(MAPA)

for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label().startswith('CIDADE_'):
        unreal.EditorLevelLibrary.destroy_actor(ator)

malha_predio = carregar('/Game/AI_Assets/prop/predio_alto.predio_alto')
malha_casulo = carregar('/Game/AI_Assets/prop/casulo.casulo')
malha_predio_baixo = carregar('/Game/AI_Assets/prop/predio_baixo.predio_baixo')
material_chao = carregar('/Game/AI_Assets/materials/M_ChaoDaCidade.M_ChaoDaCidade')
cubo = carregar('/Engine/BasicShapes/Cube.Cube')
# A torre gerada (texto -> 3D) e o bloco de engine: a malha do predio baixo ainda sai em laminas e
# entra na cidade quando o tratamento dela fechar (o bloco de engine da leitura de quarteirao agora).
malhas = [m for m in (malha_predio, malha_predio_baixo, cubo) if m]

# o piso da praca recebe a textura de chao industrial da cidade
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label() == 'MUNDO_Piso' and material_chao:
        ator.static_mesh_component.set_material(0, material_chao)
        saida.append('piso da praca com a textura da cidade')
materiais = [m for m in (carregar('/Game/AI_Assets/materials/M_LataoPolido.M_LataoPolido'),
                         carregar('/Game/AI_Assets/materials/M_AcoEscuro.M_AcoEscuro'),
                         carregar('/Game/AI_Assets/materials/M_PedraRestaurada.M_PedraRestaurada')) if m]
saida.append('malhas: ' + str(len(malhas)) + ' | materiais: ' + str(len(materiais)))

criados = 0
meio = (LADO - 1) / 2.0
for ix in range(LADO):
    for iy in range(LADO):
        x = (ix - meio) * PASSO
        y = (iy - meio) * PASSO
        # a praca central (onde o jogador nasce) fica limpa
        if math.hypot(x, y) < RECUO:
            continue

        # jitter: cidade de verdade nao e tabuleiro
        px = x + random.uniform(-800.0, 800.0)
        py = y + random.uniform(-800.0, 800.0)
        giro = random.choice([0.0, 90.0, 180.0, 270.0]) + random.uniform(-6.0, 6.0)
        altura = random.uniform(14.0, 34.0)          # escala: a malha vem com ~2 m
        malha = random.choice(malhas)

        ator = unreal.EditorLevelLibrary.spawn_actor_from_class(
            # ATENCAO: unreal.Rotator(a, b, c) e (roll, PITCH, YAW). Passar o yaw no segundo argumento
            # deita o predio - foi esse o bug que deixou as torres deitadas por varias rodadas.
            unreal.StaticMeshActor, unreal.Vector(px, py, 0.0), unreal.Rotator(0.0, 0.0, giro))
        comp = ator.static_mesh_component
        comp.set_static_mesh(malha)
        comp.set_mobility(unreal.ComponentMobility.MOVABLE)
        # A torre e esguia (XY menor que Z); o bloco de engine e largo e baixo - dois tipos de volume.
        if malha == cubo:
            ator.set_actor_scale3d(unreal.Vector(altura * 0.9, altura * 0.75, altura * 0.45))
        else:
            ator.set_actor_scale3d(unreal.Vector(altura * 0.55, altura * 0.55, altura))
        if materiais:
            comp.set_material(0, random.choice(materiais))
        ator.set_actor_label('CIDADE_%02d%02d' % (ix, iy))
        criados += 1

saida.append('predios criados: ' + str(criados))
saida.append('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

# medicao imediata, no MESMO processo: elimina duvida de persistencia entre processos
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label().startswith('CIDADE_'):
        origem, extensao = ator.get_actor_bounds(False)
        escala = ator.get_actor_scale3d()
        saida.append('MEDIDO %s escala (%.1f, %.1f, %.1f) caixa %.0f x %.0f x %.0f m' % (
            ator.get_actor_label(), escala.x, escala.y, escala.z,
            extensao.x * 2 / 100, extensao.y * 2 / 100, extensao.z * 2 / 100))
        if len([s for s in saida if s.startswith('MEDIDO')]) >= 4:
            break

with open(unreal.Paths.project_saved_dir() + 'Cidade.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
