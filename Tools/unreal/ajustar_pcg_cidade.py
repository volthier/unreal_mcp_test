# -*- coding: utf-8 -*-
'''Ajusta o PCG da cidade: area do volume, espacamento da grade, e CONTA o que foi gerado.

Excecao EXC-004: o MCP nao escreve a lista de malhas do Static Mesh Spawner nem o CellSize da grade (o
CellSize e StructProperty e recusa int, Vector2D e Vector). Este script e o caminho que funciona - e ele
MEDE o resultado pela saida do proprio componente de PCG, em vez de eu supor.
'''

import unreal

MAPA = '/Game/Maps/MundoAberto'
GRAFO = '/Game/AI_Assets/pcg/PCG_CidadeKardys'
LADO_M = 800.0        # a area inicial tem ~800 m de canto a canto
PASSO_CM = 4200.0     # 42 m entre quarteiroes
saida = []


def registrar(t):
    saida.append(str(t))
    print('  ' + str(t), flush=True)


unreal.EditorLevelLibrary.load_level(MAPA)
grafo = unreal.EditorAssetLibrary.load_asset(GRAFO)
registrar('grafo: ' + ('carregado' if grafo else 'AUSENTE'))

# 1. o espacamento da grade: tenta os tipos plausiveis e registra qual aceitou
for no in grafo.get_nodes() if hasattr(grafo, 'get_nodes') else []:
    ajustes = no.get_settings()
    if ajustes is None or 'Grid' not in ajustes.get_class().get_name():
        continue
    for tipo, valor in (('Vector', unreal.Vector(PASSO_CM, PASSO_CM, 0.0)),
                        ('Vector2D', unreal.Vector2D(PASSO_CM, PASSO_CM)),
                        ('float', PASSO_CM)):
        try:
            ajustes.set_editor_property('cell_size', valor)
            registrar('cell_size aceito como %s' % tipo)
            break
        except Exception as erro:
            registrar('cell_size recusou %s (%s)' % (tipo, str(erro)[:60]))

unreal.EditorAssetLibrary.save_asset(GRAFO)

# 2. o volume de PCG: cobre a area inicial inteira, nao 60 m
volume = None
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label().startswith('CIDADE_PCG'):
        volume = ator
if volume is None:
    volume = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PCGVolume, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
    volume.set_actor_label('CIDADE_PCG_Volume')
volume.set_actor_location(unreal.Vector(0, 0, 200), False, False)
volume.set_actor_scale3d(unreal.Vector(LADO_M, LADO_M, 60.0))
registrar('volume em %s com escala %s' % (volume.get_actor_location(), volume.get_actor_scale3d()))

componente = volume.get_component_by_class(unreal.PCGComponent)
if componente:
    try:
        componente.set_graph(grafo)
    except Exception as erro:
        registrar('set_graph: ' + str(erro)[:80])
    componente.generate(True)
    registrar('PCG executado')
    try:
        dados = componente.get_generated_graph_output()
        pontos = dados.get_points() if dados else None
        registrar('pontos gerados: %s' % (len(pontos) if pontos is not None else 'sem dados'))
    except Exception as erro:
        registrar('contagem: ' + str(erro)[:90])
else:
    registrar('volume sem PCGComponent')

registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'PCGAjuste.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
