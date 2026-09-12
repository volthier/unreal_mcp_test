# -*- coding: utf-8 -*-
# FRAME 3D DO LOGIN (spec secao 4: SM_LoginFrame_*) - construido e RENDERIZADO no Blender.
#
# Por que renderizar em vez de usar a mesh dentro do UMG: a propria secao 32 autoriza - "se o frame 3D nao
# produzir beneficio perceptivel comparado a um sistema 9-slice renderizado, utilize o Blender para gerar o
# frame high-quality e faca bake/render em pecas modulares para utilizacao eficiente no UMG". Mesh 3D dentro de
# widget custa draw call e nao aceita o corte 9-slice; o RENDER traz o bevel, o chanfro e o brilho reais e
# entra como brush Box, que e como a UI escala.
#
# Pecas pedidas na secao: outer, inner, glass, 4 cantos, top trim e bottom trim. Materiais separados: metal,
# dark metal, glass, ciano emissivo e magenta emissivo.
#
# Roda por linha de comando (excecao EXC-002, com o MCP do Blender fora do ar):
#   blender --background --python Tools/Blender/frame_login.py

import bpy
import math
import os

SAIDA = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/ui/frame_blender.png'

bpy.ops.wm.read_factory_settings(use_empty=True)


def material(nome, cor, metalico, rugosidade, emissivo=None, forca=0.0, alpha=1.0):
    mat = bpy.data.materials.new(nome)
    mat.use_nodes = True
    bsdf = mat.node_tree.nodes['Principled BSDF']
    bsdf.inputs['Base Color'].default_value = (cor[0], cor[1], cor[2], 1.0)
    bsdf.inputs['Metallic'].default_value = metalico
    bsdf.inputs['Roughness'].default_value = rugosidade
    if emissivo:
        bsdf.inputs['Emission Color'].default_value = (emissivo[0], emissivo[1], emissivo[2], 1.0)
        bsdf.inputs['Emission Strength'].default_value = forca
    if alpha < 1.0:
        bsdf.inputs['Alpha'].default_value = alpha
        mat.blend_method = 'BLEND'
    return mat


# PALETA DA SPEC (secao 6): metal #071522/#142B3D, interior #050B14, ciano #00E5FF, magenta #FF2ED1
METAL = material('MI_Login_Metal', (0.055, 0.13, 0.24), 0.95, 0.24)
METAL_ESCURO = material('MI_Login_DarkMetal', (0.020, 0.043, 0.078), 0.85, 0.36)
VIDRO = material('MI_Login_Glass', (0.03, 0.07, 0.12), 0.2, 0.12, alpha=0.16)
CIANO = material('MI_Login_CyanEmissive', (0.0, 0.1, 0.15), 0.0, 0.3, emissivo=(0.0, 0.9, 1.0), forca=6.0)
MAGENTA = material('MI_Login_MagentaEmissive', (0.15, 0.0, 0.1), 0.0, 0.3, emissivo=(1.0, 0.18, 0.82), forca=5.0)

L, A, P = 1.0, 1.0, 0.075   # largura, altura e profundidade (a spec pede peca FINA, mas com relevo visivel)
BORDA = 0.055               # espessura da blindagem
CORTE = 0.085               # o corte de canto a 45 graus: maior que a borda, senao o chanfro some


def peca(nome, tamanho, local, mat, bevel=0.006, rot=0.0):
    bpy.ops.mesh.primitive_cube_add(size=1, location=local)
    obj = bpy.context.active_object
    obj.name = nome
    obj.scale = (tamanho[0], tamanho[1], tamanho[2])
    obj.rotation_euler = (0, rot, 0)
    bpy.ops.object.transform_apply(scale=True, rotation=True)
    mod = obj.modifiers.new('chanfro', 'BEVEL')
    mod.width = bevel
    mod.segments = 4
    mod.limit_method = 'ANGLE'
    obj.data.materials.append(mat)
    return obj


# LAYER 05: a blindagem externa em quatro barras (a janela do painel e o vao)
# As barras sao ENCURTADAS pelo tamanho do corte: assim os cantos ficam livres para as pecas a 45 graus, e o
# chanfro aparece. Na primeira versao elas iam de ponta a ponta e COBRIAM o canto - o frame ficou quadrado.
peca('SM_LoginFrame_Outer_Top', (L - 2 * CORTE, BORDA, P), (0, A / 2 - BORDA / 2, 0), METAL)
peca('SM_LoginFrame_Outer_Bottom', (L - 2 * CORTE, BORDA, P), (0, -A / 2 + BORDA / 2, 0), METAL)
peca('SM_LoginFrame_Outer_Left', (BORDA, A - 2 * CORTE, P), (-L / 2 + BORDA / 2, 0, 0), METAL)
peca('SM_LoginFrame_Outer_Right', (BORDA, A - 2 * CORTE, P), (L / 2 - BORDA / 2, 0, 0), METAL)

# LAYER 01/02/03: placa de fundo e placa interna rebaixada (o miolo escuro)
peca('SM_LoginFrame_Backplate', (L - 2 * CORTE, A - 2 * CORTE, P * 0.5), (0, 0, -P * 0.3), METAL_ESCURO, 0.004)
peca('SM_LoginFrame_Inner', (L - 2 * BORDA - 0.03, A - 2 * BORDA - 0.03, P * 0.35), (0, 0, -P * 0.1), METAL_ESCURO, 0.003)

# LAYER 04: a superficie de vidro sobre o miolo
peca('SM_LoginFrame_Glass', (L - 2 * BORDA - 0.04, A - 2 * BORDA - 0.04, P * 0.12), (0, 0, P * 0.18), VIDRO, 0.002)

# OS QUATRO CANTOS CORTADOS: barras a 45 graus, com trim ciano
for nome, sx, sy in (('TL', -1, 1), ('TR', 1, 1), ('BL', -1, -1), ('BR', 1, -1)):
    comprimento = CORTE * 1.9
    peca('SM_LoginFrame_Corner' + nome, (comprimento, BORDA, P),
         (sx * (L / 2 - CORTE * 0.85), sy * (A / 2 - CORTE * 0.85), 0), METAL, 0.005,
         rot=sx * sy * math.radians(-45))
    peca('SM_LoginFrame_CornerTrim' + nome, (comprimento * 0.6, BORDA * 0.30, P * 1.10),
         (sx * (L / 2 - CORTE * 0.85), sy * (A / 2 - CORTE * 0.85), 0), CIANO, 0.002,
         rot=sx * sy * math.radians(-45))
    peca('SM_LoginFrame_CornerAccent' + nome, (comprimento * 0.35, BORDA * 0.22, P * 1.15),
         (sx * (L / 2 - CORTE * 1.55), sy * (A / 2 - CORTE * 1.55), 0), MAGENTA, 0.002,
         rot=sx * sy * math.radians(-45))

# TRIMS: ciano ao longo do topo e da base (o que a secao chama de cyan emissive trim)
peca('SM_LoginFrame_TopTrim', (L - 2 * CORTE, BORDA * 0.28, P * 1.08), (0, A / 2 - BORDA * 0.5, 0), CIANO, 0.002)
peca('SM_LoginFrame_BottomTrim', (L - 2 * CORTE, BORDA * 0.28, P * 1.08), (0, -A / 2 + BORDA * 0.5, 0), CIANO, 0.002)

# o recorte do topo (clipping), com trim magenta
peca('SM_LoginFrame_TopClip', (0.28, BORDA * 0.9, P * 1.25), (0, A / 2 - BORDA * 0.4, 0), METAL_ESCURO, 0.004)
peca('SM_LoginFrame_TopClipTrim', (0.22, BORDA * 0.22, P * 1.32), (0, A / 2 - BORDA * 0.5, 0), MAGENTA, 0.002)

# CAMERA ortografica de frente, para o render vir como arte de UI (sem perspectiva)
bpy.ops.object.camera_add(location=(0, 0, 3), rotation=(0, 0, 0))
cam = bpy.context.active_object
cam.data.type = 'ORTHO'
cam.data.ortho_scale = 1.16
bpy.context.scene.camera = cam

# luz de estudio: uma chave fria, um preenchimento e um rim
bpy.ops.object.light_add(type='AREA', location=(-1.4, 1.2, 1.8))
bpy.context.active_object.data.energy = 220
bpy.context.active_object.data.color = (0.55, 0.8, 1.0)
bpy.ops.object.light_add(type='AREA', location=(1.6, -1.0, 1.2))
bpy.context.active_object.data.energy = 90
bpy.context.active_object.data.color = (1.0, 0.5, 0.9)

# mundo escuro, para o emissivo aparecer
mundo = bpy.data.worlds.new('W')
mundo.use_nodes = True
mundo.node_tree.nodes['Background'].inputs[0].default_value = (0.004, 0.008, 0.016, 1)
bpy.context.scene.world = mundo

# render: Cycles, poucas amostras com denoise, fundo transparente (a UI compoe por cima)
c = bpy.context.scene
c.render.engine = 'CYCLES'
c.cycles.samples = 48
c.cycles.use_denoising = True
c.render.film_transparent = True
c.render.resolution_x = 1024
c.render.resolution_y = 1024
c.render.image_settings.file_format = 'PNG'
c.render.image_settings.color_mode = 'RGBA'
c.render.filepath = SAIDA
bpy.ops.render.render(write_still=True)
print('frame renderizado em', SAIDA)
