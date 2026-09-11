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
malhas = [m for m in (malha_predio,) if m]   # so a torre: o casulo e um domo, nao e predio
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
            unreal.StaticMeshActor, unreal.Vector(px, py, 0.0), unreal.Rotator(0.0, giro, 0.0))
        comp = ator.static_mesh_component
        comp.set_static_mesh(malha)
        comp.set_mobility(unreal.ComponentMobility.MOVABLE)
                ator.set_actor_scale3d(unreal.Vector(altura * 0.55, altura * 0.55, altura))
        if materiais:
            comp.set_material(0, random.choice(materiais))
        ator.set_actor_label('CIDADE_%02d%02d' % (ix, iy))
        criados += 1

saida.append('predios criados: ' + str(criados))
saida.append('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'Cidade.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
