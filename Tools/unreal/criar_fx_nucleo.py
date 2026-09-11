# -*- coding: utf-8 -*-
'''Cria o sistema de FX da aura do nucleo, partindo de um template FUNCIONAL do engine.

Por que duplicar em vez de criar do zero: criar do zero pela API/MCP exige montar emissor, renderer e
modulos na mao (e o parametro de template da ferramenta MCP quer referencia de objeto, nao caminho).
Duplicar um template que ja funciona e o caminho curto e confiavel - e o ajuste fino (cor, material do
sprite, taxa) e feito depois, pela MCP de Niagara, que tem ferramentas de leitura e escrita.

Templates usados (engine): /Niagara/DefaultAssets/Templates/Systems/
'''

import unreal

PASTA = '/Game/AI_Assets/fx'
MODELO = '/Niagara/DefaultAssets/Templates/Systems/MinimalLightweight.MinimalLightweight'
DESTINO = PASTA + '/NS_NucleoAura'
saida = []

if unreal.EditorAssetLibrary.does_asset_exist(DESTINO):
    saida.append('sistema ja existe: ' + DESTINO)
else:
    duplicado = unreal.EditorAssetLibrary.duplicate_asset(MODELO, DESTINO)
    saida.append('duplicado de MinimalLightweight: ' + str(duplicado is not None))

sistema = unreal.EditorAssetLibrary.load_asset(DESTINO)
saida.append('sistema carregado: ' + str(sistema is not None))
if sistema:
    try:
        emissores = sistema.get_editor_property('emitter_handles')
        saida.append('emissores no sistema: ' + str(len(emissores)))
        for i, emissor in enumerate(emissores):
            saida.append('  emissor %d: %s' % (i, emissor.get_editor_property('name') if hasattr(emissor, 'get_editor_property') else str(emissor)))
    except Exception as erro:
        saida.append('ao ler emissores: ' + str(erro))

with open(unreal.Paths.project_saved_dir() + 'FxNucleo.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
