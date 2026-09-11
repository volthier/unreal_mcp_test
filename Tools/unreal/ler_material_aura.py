# -*- coding: utf-8 -*-
'''Le o material da aura DE VOLTA: quais expressoes existem, se a amostra tem textura e o que esta ligado
no emissivo. Antes de tentar qualquer ajuste visual, saber o que o material realmente e.
'''

import unreal

CAMINHO = '/Game/AI_Assets/materials/M_AuraGeada.M_AuraGeada'
saida = []

mat = unreal.EditorAssetLibrary.load_asset(CAMINHO)
if mat is None:
    saida.append('MATERIAL NAO EXISTE: ' + CAMINHO)
else:
    saida.append('material: ' + mat.get_name())
    try:
        saida.append('blend mode: ' + str(mat.get_editor_property('blend_mode')))
        saida.append('shading: ' + str(mat.get_editor_property('shading_model')))
        saida.append('two sided: ' + str(mat.get_editor_property('two_sided')))
    except Exception as erro:
        saida.append('propriedades: ' + str(erro)[:90])

    expressoes = unreal.MaterialEditingLibrary.get_material_expressions(mat)
    saida.append('expressoes: %d' % len(expressoes))
    for expressao in expressoes:
        classe = expressao.get_class().get_name()
        detalhe = ''
        for campo in ('parameter_name', 'texture', 'constant', 'default_value', 'r', 'g'):
            try:
                valor = expressao.get_editor_property(campo)
                if valor is not None:
                    detalhe += ' %s=%s' % (campo, str(valor)[:44])
            except Exception:
                continue
        saida.append('   %s%s' % (classe, detalhe))

    # o que esta ligado no emissivo?
    for propriedade, nome in ((unreal.MaterialProperty.MP_EMISSIVE_COLOR, 'EMISSIVO'),
                              (unreal.MaterialProperty.MP_BASE_COLOR, 'BASE')):
        try:
            ligadas = unreal.MaterialEditingLibrary.get_inputs_for_material_property(mat, propriedade)
            saida.append('%s ligado em: %s' % (nome, [e.get_class().get_name() for e in ligadas]))
        except Exception as erro:
            saida.append('%s: %s' % (nome, str(erro)[:80]))

# as instancias existem e apontam para este material?
for nome in ('Charger', 'Cryonix'):
    caminho = '/Game/AI_Assets/materials/MI_Aura_%s.MI_Aura_%s' % (nome, nome)
    instancia = unreal.EditorAssetLibrary.load_asset(caminho)
    if instancia is None:
        saida.append('instancia ausente: ' + caminho)
    else:
        try:
            pai = instancia.get_editor_property('parent')
            cor = instancia.get_vector_parameter_value(unreal.MaterialParameterInfo('CorDaAura')) if hasattr(instancia, 'get_vector_parameter_value') else 'n/d'
            saida.append('instancia %s: pai=%s cor=%s' % (nome, pai.get_name() if pai else 'nenhum', cor))
        except Exception as erro:
            saida.append('instancia %s: %s' % (nome, str(erro)[:90]))

with open(unreal.Paths.project_saved_dir() + 'LerAura.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
