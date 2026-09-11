# -*- coding: utf-8 -*-
'''Gera a LAMINA DE AGUA do rio e as duas partes da ARVORE (tronco e copa).

Excecao registrada EXC-002 (como o terreno): o MCP do Unreal importa malha e cria material, mas nao tem
ferramenta de modelagem - entao a geometria nasce aqui e entra no editor PELO MCP.

A agua segue exatamente a MESMA curva do leito do terreno (x = 120 * sin(y / 260)), para nao encostar na
margem. O nivel fica 2,5 m acima do fundo (que o terreno cava a -11 m), o que da lamina com profundidade de
leitura sem inundar o vale.

A arvore vem em DUAS malhas separadas (tronco e copa) de proposito: assim cada parte recebe o seu material
pelo proprio MCP (StaticMeshTools.set_material), em vez de uma malha unica com cor errada.
'''

from __future__ import annotations

import math
import sys

import bpy
import bmesh

COMPRIMENTO = 800.0
PASSO = 8.0
MEIA_LARGURA = 30.0
NIVEL_AGUA = -8.5


def limpar() -> None:
    for objeto in list(bpy.data.objects):
        bpy.data.objects.remove(objeto, do_unlink=True)


def exportar(objeto, destino: str) -> None:
    bpy.ops.object.select_all(action='DESELECT')
    objeto.select_set(True)
    bpy.context.view_layer.objects.active = objeto
    bpy.ops.export_scene.fbx(filepath=destino, use_selection=True, apply_unit_scale=True, mesh_smooth_type='FACE')


def criar_rio(destino: str) -> None:
    malha = bpy.data.meshes.new('RioKardys')
    objeto = bpy.data.objects.new('RioKardys', malha)
    bpy.context.collection.objects.link(objeto)
    bm = bmesh.new()
    esquerda, direita = [], []
    y = -COMPRIMENTO / 2.0
    while y <= COMPRIMENTO / 2.0:
        centro = 120.0 * math.sin(y / 260.0)
        esquerda.append(bm.verts.new((centro - MEIA_LARGURA, y, NIVEL_AGUA)))
        direita.append(bm.verts.new((centro + MEIA_LARGURA, y, NIVEL_AGUA)))
        y += PASSO
    for i in range(len(esquerda) - 1):
        try:
            bm.faces.new((esquerda[i], direita[i], direita[i + 1], esquerda[i + 1]))
        except ValueError:
            continue
    bm.normal_update()
    bm.to_mesh(malha)
    bm.free()
    uvs = malha.uv_layers.new(name='UVMap')
    for face in malha.polygons:
        for laco in face.loop_indices:
            co = malha.vertices[malha.loops[laco].vertex_index].co
            uvs.data[laco].uv = (co.x / 120.0, co.y / 60.0)
    exportar(objeto, destino)
    print('rio exportado:', destino, '| faces:', len(malha.polygons))


def criar_arvore(destino_tronco: str, destino_copa: str) -> None:
    limpar()
    bm = bmesh.new()
    bmesh.ops.create_cone(bm, cap_ends=True, segments=6, radius1=0.35, radius2=0.22, depth=3.2)
    for vertice in bm.verts:
        vertice.co.z += 1.6
    tronco_malha = bpy.data.meshes.new('ArvoreTronco')
    bm.to_mesh(tronco_malha)
    bm.free()
    tronco = bpy.data.objects.new('ArvoreTronco', tronco_malha)
    bpy.context.collection.objects.link(tronco)
    exportar(tronco, destino_tronco)
    print('tronco exportado:', destino_tronco)

    limpar()
    bm = bmesh.new()
    bmesh.ops.create_icosphere(bm, subdivisions=1, radius=2.4)
    bmesh.ops.scale(bm, vec=(1.0, 1.0, 0.85), verts=bm.verts)
    for vertice in bm.verts:
        vertice.co.z += 3.8
    copa_malha = bpy.data.meshes.new('ArvoreCopa')
    bm.to_mesh(copa_malha)
    bm.free()
    copa = bpy.data.objects.new('ArvoreCopa', copa_malha)
    bpy.context.collection.objects.link(copa)
    exportar(copa, destino_copa)
    print('copa exportada:', destino_copa)


def principal() -> int:
    limpar()
    criar_rio(sys.argv[-3])
    criar_arvore(sys.argv[-2], sys.argv[-1])
    return 0


if __name__ == '__main__':
    raise SystemExit(principal())
