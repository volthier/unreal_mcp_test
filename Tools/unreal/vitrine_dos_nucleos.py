# -*- coding: utf-8 -*-
'''Monta a VITRINE dos dez nucleos no mapa: cristal + aura de gelo + luz, cada um na cor do seu chassi.

Por que existe: a vitrine do jogo vive no widget C++ (aba CHASSI mostra o cristal girando). Enquanto o
editor do autor nao for reiniciado, esse codigo novo nao roda em tela - entao esta vitrine de teste coloca
no mapa exatamente os mesmos tres elementos (malha do cristal, material MI_Nucleo_<Chassi>, casca de aura
com M_AuraGeada tingida) para a leitura ser verificavel AGORA, com os dez lado a lado.

A cor sai do Data/DT_Chassis.csv (coluna CoreColor) - dado, nao gosto.'''''

import csv
import math
import unreal

MAPA = '/Game/Maps/MundoAberto'
CSV = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Data/DT_Chassis.csv'
PASTA_MAT = '/Game/AI_Assets/materials'
saida = []


def registrar(t):
    saida.append(str(t))


def cores_do_csv():
    linhas = []
    with open(CSV, encoding='utf-8-sig') as arquivo:
        for linha in csv.DictReader(arquivo):
            texto = (linha.get('CoreColor') or '').strip().strip('()')
            valores = {}
            for parte in texto.split(','):
                if '=' in parte:
                    chave, valor = parte.split('=', 1)
                    valores[chave.strip()] = float(valor)
            if {'R', 'G', 'B'} <= set(valores):
                linhas.append((linha['Name'], unreal.LinearColor(valores['R'], valores['G'], valores['B'], 1.0)))
    return linhas


unreal.EditorLevelLibrary.load_level(MAPA)
for ator in unreal.EditorLevelLibrary.get_all_level_actors():
    if ator.get_actor_label().startswith('VITRINE_'):
        unreal.EditorLevelLibrary.destroy_actor(ator)

cristal = unreal.EditorAssetLibrary.load_asset('/Game/AI_Assets/prop/cristal_cryonix.cristal_cryonix')
plano = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Plane.Plane')
cilindro = unreal.EditorAssetLibrary.load_asset('/Engine/BasicShapes/Cylinder.Cylinder')
base_aura = unreal.EditorAssetLibrary.load_asset(PASTA_MAT + '/M_AuraGeada.M_AuraGeada')
latao = unreal.EditorAssetLibrary.load_asset(PASTA_MAT + '/M_LataoPolido.M_LataoPolido')
registrar('malhas/materiais: cristal=%s plano=%s aura=%s' % (cristal is not None, plano is not None, base_aura is not None))

chassis = cores_do_csv()
largura = 900.0
inicio = -(len(chassis) - 1) * largura / 2.0

for indice, (nome, cor) in enumerate(chassis):
    x = inicio + indice * largura
    # 1. pedestal de latao, para ler como peca de restauro (o canonico de Kardys)
    pedestal = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(x, 0.0, 60.0), unreal.Rotator(0, 0, 0))
    pedestal.set_actor_label('VITRINE_%s_Pedestal' % nome)
    pedestal.static_mesh_component.set_static_mesh(cilindro)
    pedestal.set_actor_scale3d(unreal.Vector(1.3, 1.3, 1.2))
    if latao:
        pedestal.static_mesh_component.set_material(0, latao)

    # 2. o cristal com a cor DAQUELE chassi
    ator = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(x, 0.0, 260.0), unreal.Rotator(0, 0, 0))
    ator.set_actor_label('VITRINE_%s_Cristal' % nome)
    ator.static_mesh_component.set_static_mesh(cristal)
    ator.set_actor_scale3d(unreal.Vector(1.0, 1.0, 1.0))   # cristal em tamanho de leitura (era 0.5: ficava um ponto)
    material = unreal.EditorAssetLibrary.load_asset('%s/MI_Nucleo_%s.MI_Nucleo_%s' % (PASTA_MAT, nome, nome))
    if material:
        ator.static_mesh_component.set_material(0, material)
    else:
        registrar('  sem material para ' + nome)

    # 3. a aura: TRES PLANOS CRUZADOS, nao uma esfera. Casca esferica sempre le como orbe (foi o que a captura
    #    mostrou); planos com material aditivo e textura de geada leem como nevoa, que e o pedido do autor.
    for indice_plano, giro in enumerate((0.0, 60.0, 120.0)):
        # O plano do engine nasce DEITADO (normal para cima): visto de lado ele fica de perfil e some. O pitch
        # de 90 graus poe ele EM PE, e o giro de 60 graus entre os tres faz a nevoa existir de qualquer angulo.
        casca = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, unreal.Vector(x, 0.0, 260.0), unreal.Rotator(roll=0.0, pitch=90.0, yaw=giro))
        casca.set_actor_label('VITRINE_%s_Aura%d' % (nome, indice_plano))
        casca.static_mesh_component.set_static_mesh(plano)
        casca.set_actor_scale3d(unreal.Vector(2.8, 2.8, 2.8))   # a nuvem de gelo tem de envolver o cristal, nao colar nele
    # A aura usa uma INSTANCIA DE MATERIAL por chassi (MI_Aura_<Chassi>): o padrao que ja funciona para o
    # nucleo. Instancia dinamica em runtime nao existe nesta versao da API de Python, e a instancia salva
    # ainda tem a vantagem de ser dado versionado, nao estado de memoria.
    if base_aura:
        nome_aura = 'MI_Aura_' + nome
        caminho_aura = PASTA_MAT + '/' + nome_aura + '.' + nome_aura
        instancia = unreal.EditorAssetLibrary.load_asset(caminho_aura)
        if instancia is None:
            ferramentas = unreal.AssetToolsHelpers.get_asset_tools()
            instancia = ferramentas.create_asset(nome_aura, PASTA_MAT, unreal.MaterialInstanceConstant,
                                                  unreal.MaterialInstanceConstantFactoryNew())
            if instancia:
                unreal.MaterialEditingLibrary.set_material_instance_parent(instancia, base_aura)
                unreal.MaterialEditingLibrary.set_material_instance_vector_parameter_value(instancia, 'CorDaAura', cor)
                unreal.EditorAssetLibrary.save_asset(PASTA_MAT + '/' + nome_aura)
        if instancia:
            casca.static_mesh_component.set_material(0, instancia)

    # 4. luz na cor do nucleo: e ela que faz a leitura da cor a distancia
    luz = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.PointLight, unreal.Vector(x, 0.0, 210.0), unreal.Rotator(0, 0, 0))
    luz.set_actor_label('VITRINE_%s_Luz' % nome)
    try:
        componente = luz.get_component_by_class(unreal.PointLightComponent)
        componente.set_light_color(cor)
        componente.set_intensity(320.0)
        componente.set_attenuation_radius(450.0)
        componente.set_cast_shadows(False)
    except Exception as erro:
        registrar('  luz de %s: %s' % (nome, str(erro)[:80]))

registrar('vitrine montada com %d nucleos' % len(chassis))
registrar('mapa salvo: ' + str(unreal.EditorLoadingAndSavingUtils.save_current_level()))

with open(unreal.Paths.project_saved_dir() + 'Vitrine.txt', 'w') as arquivo:
    arquivo.write(chr(10).join(saida) + chr(10))
