# -*- coding: utf-8 -*-
'''Sonda 2: como montar o grafo de PCG por Python (metodos do graph + classes de node/settings).'''

import unreal

saida = []
grafo_cls = unreal.PCGGraph
metodos = [m for m in dir(grafo_cls) if not m.startswith('_') and ('node' in m.lower() or 'edge' in m.lower() or 'parameter' in m.lower())]
saida.append('metodos do PCGGraph: ' + ', '.join(metodos))

for nome in ('PCGCreatePointsGridSettings', 'PCGStaticMeshSpawnerSettings', 'PCGMeshSelectorWeighted',
             'PCGMeshSelectorWeightedEntry', 'PCGTransformPointsSettings', 'PCGGraphInstance'):
    saida.append('%s: %s' % (nome, 'existe' if hasattr(unreal, nome) else 'nao existe'))

campos = [c for c in dir(unreal.PCGStaticMeshSpawnerSettings) if not c.startswith('_')] if hasattr(unreal, 'PCGStaticMeshSpawnerSettings') else []
saida.append('campos do StaticMeshSpawnerSettings: ' + ', '.join(campos[:20]))

campos_grade = [c for c in dir(unreal.PCGCreatePointsGridSettings) if not c.startswith('_')] if hasattr(unreal, 'PCGCreatePointsGridSettings') else []
saida.append('campos do CreatePointsGridSettings: ' + ', '.join(campos_grade[:20]))

campos_comp = [c for c in dir(unreal.PCGComponent) if not c.startswith('_') and ('graph' in c.lower() or 'generat' in c.lower())]
saida.append('PCGComponent (graph/gerar): ' + ', '.join(campos_comp[:14]))

with open(unreal.Paths.project_saved_dir() + 'SondarPCG2.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
