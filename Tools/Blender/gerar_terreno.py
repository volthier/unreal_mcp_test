# -*- coding: utf-8 -*-
'''Gera o TERRENO do mapa inicial: relevo, vale de rio e ruas planas.

Por que existe (e por que e excecao registrada EXC-002): o MCP do Unreal tem import de malha
(StaticMeshTools.import_file), mas NAO tem ferramenta de Landscape - nao existe toolset de terreno nos 59.
Entao a malha do terreno nasce aqui, no Blender, e entra no editor PELO MCP. O caminho padrao do Blender seria
execute_blender_code; nesta maquina o addon BLENDERMCP nao esta ligado, e o CLI e o registrado.

O que ele faz, com numeros:
- malha de 800 x 800 m no centro do mundo (a cidade ocupa ~325 m, sobra cinturao verde e rio);
- relevo por ruido em 4 oitavas (amplitude 34 m) - colina de verdade, nao laje;
- um VALE DE RIO com fundo plano, seguindo uma curva senoidal, largura 60 m e profundidade 11 m;
- RUAS planas em grade, largura 14 m, para os quarteiroes assentarem;
- exporta FBX em metros (a UE converte para cm).

Uso: blender --background --python Tools/Blender/gerar_terreno.py -- <saida.fbx>
'''

from __future__ import annotations

import math
import sys

import bpy
import bmesh
from mathutils import noise

LADO = 800.0        # metros
DIVISOES = 220       # ~40 mil faces
AMPLITUDE = 34.0    # altura das colinas
LARGURA_RIO = 60.0
PROFUNDIDADE_RIO = 11.0
LARGURA_RUA = 14.0
PASSO_RUA = 190.0


def altura(x: float, y: float) -> float:
    # relevo: quatro oitavas de ruido, escala grande para dar morro e nao serrilha
    h = 0.0
    escala = 0.0016
    peso = 1.0
    for _ in range(4):
        h += noise.noise((x * escala, y * escala, 0.0)) * peso
        escala *= 2.1
        peso *= 0.5
    h *= AMPLITUDE

    # vale do rio: o leito segue uma senoide e o terreno cai suavemente ate ele
    leito = 120.0 * math.sin(y / 260.0) + 0.0
    distancia = abs(x - leito)
    if distancia < LARGURA_RIO * 2.5:
        queda = max(0.0, 1.0 - (distancia / (LARGURA_RIO * 2.5))) ** 1.6
        h -= PROFUNDIDADE_RIO * queda
        if distancia < LARGURA_RIO * 0.5:
            h = -PROFUNDIDADE_RIO  # fundo plano: a agua tem onde correr

    # ruas: faixas planas em grade, para o quarteirao assentar sem degrau
    perto_x = abs((x % PASSO_RUA) - PASSO_RUA / 2.0) > (PASSO_RUA / 2.0 - LARGURA_RUA)
    perto_y = abs((y % PASSO_RUA) - PASSO_RUA / 2.0) > (PASSO_RUA / 2.0 - LARGURA_RUA)
    if perto_x or perto_y:
        h = min(h, 4.0)
    return h


def principal() -> int:
    for objeto in list(bpy.data.objects):
        bpy.data.objects.remove(objeto, do_unlink=True)

    malha = bpy.data.meshes.new('TerrenoKardys')
    objeto = bpy.data.objects.new('TerrenoKardys', malha)
    bpy.context.collection.objects.link(objeto)

    bm = bmesh.new()
    meio = LADO / 2.0
    passo = LADO / DIVISOES
    grade = {}
    for i in range(DIVISOES + 1):
        for j in range(DIVISOES + 1):
            x = -meio + i * passo
            y = -meio + j * passo
            grade[(i, j)] = bm.verts.new((x, y, altura(x, y)))
    bm.verts.ensure_lookup_table()

    for i in range(DIVISOES):
        for j in range(DIVISOES):
            try:
                bm.faces.new((grade[(i, j)], grade[(i + 1, j)], grade[(i + 1, j + 1)], grade[(i, j + 1)]))
            except ValueError:
                continue
    bm.normal_update()
    bm.to_mesh(malha)
    bm.free()

    # UV simples, para o material do terreno nao ficar esticado
    uvs = malha.uv_layers.new(name='UVMap')
    for face in malha.polygons:
        for laco in face.loop_indices:
            vertice = malha.vertices[malha.loops[laco].vertex_index].co
            uvs.data[laco].uv = ((vertice.x + meio) / LADO, (vertice.y + meio) / LADO)

    destino = sys.argv[-1]
    bpy.ops.object.select_all(action='DESELECT')
    objeto.select_set(True)
    bpy.context.view_layer.objects.active = objeto
    bpy.ops.export_scene.fbx(filepath=destino, use_selection=True, apply_unit_scale=True, mesh_smooth_type='FACE')
    print('terreno exportado:', destino)
    print('faces:', len(malha.polygons), '| vertices:', len(malha.vertices))
    print('amplitude medida: %.1f m' % (max(v.co.z for v in malha.vertices) - min(v.co.z for v in malha.vertices)))
    return 0


if __name__ == '__main__':
    raise SystemExit(principal())
