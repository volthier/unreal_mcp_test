# -*- coding: utf-8 -*-
'''Cria o grafo de PCG da cidade POR PYTHON (salvo em disco) e o executa no mapa do mundo.

Por que por Python e nao pela MCP: a MCP do Unreal cria o asset na MEMORIA do editor e nao salva - o que
nao esta em disco nao existe para o pipeline headless nem para o proximo restart. Aqui o grafo e criado,
configurado e SALVO, e depois um PCGVolume no mapa do mundo o executa de verdade.

Cadeia do grafo (a mesma que a MCP montou, agora em disco):
  Create Points Grid (a malha urbana) -> Transform Points (variacao) -> Static Mesh Spawner (os predios)
'''

import unreal

GRAFO = '/Game/AI_Assets/pcg/PCG_CidadeKardys'
MAPA = '/Game/Maps/MundoAberto'
PASTA = '/Game/AI_Assets/pcg'
saida = []


def registrar(t):
    saida.append(str(t))
    print('  ' + str(t), flush=True)


grafo = unreal.EditorAssetLibrary.load_asset(GRAFO)
if grafo is None:
    ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
    grafo = ferramentas.create_asset('PCG_CidadeKardys', PASTA, unreal.PCGGraph, unreal.PCGGraphFactory())
    registrar('grafo criado: ' + str(grafo is not None))
else:
    registrar('grafo ja existia em disco')

if grafo is None:
    raise SystemExit('sem grafo')

# 1. os tres nos da cidade. ATENCAO: add_node_of_type devolve uma TUPLA (no, settings) - o settings ja vem
#    pronto, nao precisa (nem existe) get_settings() no retorno.
no_grade, ajustes_grade = grafo.add_node_of_type(unreal.PCGCreatePointsGridSettings)
no_variacao, ajustes_variacao = grafo.add_node_of_type(unreal.PCGTransformPointsSettings)
no_predios, ajustes_predios = grafo.add_node_of_type(unreal.PCGStaticMeshSpawnerSettings)
registrar('nos: grade=%s variacao=%s predios=%s' % (no_grade is not None, no_variacao is not None, no_predios is not None))

# 2. liga grade -> variacao -> predios
for origem, destino in ((no_grade, no_variacao), (no_variacao, no_predios)):
    if origem is None or destino is None:
        continue
    try:
        grafo.add_edge(origem, 'Out', destino, 'In')
        registrar('ligacao feita')
    except Exception as erro:
        registrar('ligacao falhou: ' + str(erro)[:120])

# 3. a malha urbana: quarteiroes de 40 m
if ajustes_grade:
    try:
        # CellSize e VECTOR (x, y), nao inteiro - e o tamanho do quarteirao em cm nos dois eixos.
        ajustes_grade.set_editor_property('cell_size', unreal.Vector2D(4000.0, 4000.0))
        registrar('cell_size = 4000 (40 m entre quarteiroes)')
    except Exception as erro:
        registrar('cell_size falhou: ' + str(erro)[:120])

# 4. os predios: seletor ponderado com as malhas do projeto
if ajustes_predios:
    try:
        seletor = unreal.PCGMeshSelectorWeighted()
        entradas = []
        for caminho, peso in (('/Game/AI_Assets/prop/predio_alto.predio_alto', 3.0),
                              ('/Game/AI_Assets/prop/predio_baixo.predio_baixo', 2.0)):
            malha = unreal.EditorAssetLibrary.load_asset(caminho)
            if malha is None:
                continue
            entrada = unreal.PCGMeshSelectorWeightedEntry()
            # O campo 'mesh' da entrada e protegido: o caminho e o DESCRITOR. Descobre a classe em execucao.
            campos = [c for c in dir(entrada) if 'descri' in c.lower() or 'mesh' in c.lower()]
            registrar('campos da entrada: ' + ', '.join(campos[:8]))
            try:
                descritor = None
                for nome_classe in ('PCGMeshDescriptor', 'PCGSoftMeshDescriptor'):
                    if hasattr(unreal, nome_classe):
                        descritor = getattr(unreal, nome_classe)()
                        break
                if descritor is not None:
                    for campo in ('mesh', 'static_mesh', 'soft_mesh'):
                        try:
                            descritor.set_editor_property(campo, malha)
                            break
                        except Exception:
                            continue
                    entrada.set_editor_property('descriptor', descritor)
                entrada.set_editor_property('weight', peso)
            except Exception as erro:
                registrar('entrada de malha: ' + str(erro)[:140])
            entradas.append(entrada)
        seletor.set_editor_property('mesh_entries', entradas)
        # 'mesh_selector_parameters' e READ-ONLY: o caminho e definir o TIPO do seletor (o engine instancia os
        # parametros) e entao MUTAR o objeto instanciado que o getter devolve.
        ajustes_predios.set_editor_property('mesh_selector_type', unreal.PCGMeshSelectorWeighted)
        instanciado = ajustes_predios.get_editor_property('mesh_selector_parameters')
        if instanciado is not None:
            instanciado.set_editor_property('mesh_entries', entradas)
            registrar('seletor: tipo definido e %d malhas no objeto instanciado' % len(entradas))
        else:
            registrar('seletor: o engine nao devolveu o objeto de parametros')
        registrar('spawner com %d malhas ponderadas' % len(entradas))
    except Exception as erro:
        registrar('spawner: ' + str(erro)[:160])

unreal.EditorAssetLibrary.save_asset(GRAFO)
registrar('grafo salvo em disco: ' + str(unreal.EditorAssetLibrary.does_asset_exist(GRAFO)))

# 5. executa no mapa do mundo, num volume de PCG
unreal.EditorLevelLibrary.load_level(MAPA)
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label().startswith('CIDADE_PCG'):
        unreal.EditorLevelLibrary.destroy_actor(ator)

volume = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PCGVolume, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0))
volume.set_actor_label('CIDADE_PCG_Volume')
try:
    volume.set_actor_scale3d(unreal.Vector(60.0, 60.0, 8.0))
except Exception as erro:
    registrar('escala do volume: ' + str(erro)[:100])

componente = volume.get_component_by_class(unreal.PCGComponent)
if componente:
    # O componente nao aceita 'graph_instance' por set_editor_property (o atributo e protegido). Tenta as
    # vias que a API oferece, da mais direta para a menos.
    atribuiu = False
    for tentativa in ('set_graph', 'set_editor_property'):
        try:
            if tentativa == 'set_graph':
                componente.set_graph(grafo)
            else:
                componente.set_editor_property('graph', grafo)
            registrar('grafo atribuido ao componente por ' + tentativa)
            atribuiu = True
            break
        except Exception as erro:
            registrar(tentativa + ' falhou: ' + str(erro)[:110])
    if not atribuiu:
        try:
            instancia = unreal.PCGGraphInstance()
            instancia.set_editor_property('graph', grafo)
            componente.set_editor_property('graph_instance', instancia)
            registrar('grafo atribuido por graph_instance')
            atribuiu = True
        except Exception as erro:
            registrar('graph_instance falhou: ' + str(erro)[:110])
    if atribuiu:
        try:
            componente.generate(True)
            registrar('PCG executado no mapa do mundo')
        except Exception as erro:
            registrar('gerar falhou: ' + str(erro)[:160])
else:
    registrar('PCGVolume sem PCGComponent')

registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

def gravar_relatorio():
    with open(unreal.Paths.project_saved_dir() + 'PCGCidade.txt', 'w') as arquivo:
        arquivo.write(chr(10).join(saida) + chr(10))


try:
    pass
finally:
    gravar_relatorio()
with open(unreal.Paths.project_saved_dir() + 'PCGCidade.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
