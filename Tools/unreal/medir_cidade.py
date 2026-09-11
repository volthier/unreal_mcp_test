# -*- coding: utf-8 -*-
'''Mede os atores da cidade: posicao, escala e a CAIXA real do componente (em metros).'''

import unreal

unreal.EditorLevelLibrary.load_level('/Game/Maps/MundoAberto')
saida = []
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if not ator.get_actor_label().startswith('CIDADE_'):
        continue
    comp = ator.static_mesh_component
    malha = comp.get_editor_property('static_mesh') if hasattr(comp, 'get_editor_property') else None
    try:
        origem, extensao = ator.get_actor_bounds(False)
        medida = 'caixa %.0f x %.0f x %.0f m' % (extensao.x * 2 / 100, extensao.y * 2 / 100, extensao.z * 2 / 100)
    except Exception as erro:
        medida = 'sem bounds: ' + str(erro)
    escala = ator.get_actor_scale3d()
    saida.append('%s | escala (%.1f, %.1f, %.1f) | %s | malha %s' % (
        ator.get_actor_label(), escala.x, escala.y, escala.z, medida, malha.get_name() if malha else '?'))
    if len(saida) >= 5:
        break

with open(unreal.Paths.project_saved_dir() + 'MedirCidade.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
