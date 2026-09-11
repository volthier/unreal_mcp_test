# -*- coding: utf-8 -*-
'''Sonda: da para criar um grafo de PCG por Python (e salvar em disco)?'

Motivo: a MCP do Unreal cria o grafo na MEMORIA do editor (nao salva), e o pipeline headless nao ve o que
nao esta em disco. Se a API de Python permitir criar e salvar, o PCG entra no projeto de verdade.
'''

import unreal

saida = []

nomes = [n for n in dir(unreal) if 'PCG' in n]
saida.append('classes/structs com PCG: ' + ', '.join(nomes[:24]))

for candidato in ('PCGGraphFactory', 'PCGGraphFactoryNew'):
    classe = getattr(unreal, candidato, None)
    saida.append('fabrica %s: %s' % (candidato, 'existe' if classe else 'nao existe'))

classe_grafo = getattr(unreal, 'PCGGraph', None)
saida.append('PCGGraph: ' + ('existe' if classe_grafo else 'nao existe'))

classe_componente = getattr(unreal, 'PCGComponent', None)
saida.append('PCGComponent: ' + ('existe' if classe_componente else 'nao existe'))

classe_volume = [n for n in dir(unreal) if 'PCGVolume' in n or 'PCGComponent' in n]
saida.append('classe de volume/componente: ' + ', '.join(classe_volume[:8]))

with open(unreal.Paths.project_saved_dir() + 'SondarPCG.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
