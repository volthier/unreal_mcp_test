"""Inspeciona um GLB e renderiza vistas - prova visual de que a malha e 3D de verdade.

Uso: Blender --background --python Tools/Blender/inspect_and_render.py -- <glb> <saida_prefixo>
"""

import os
import sys

import bpy


def limpar_cena():
    bpy.ops.wm.read_factory_settings(use_empty=True)


def main():
    argumentos = sys.argv[sys.argv.index('--') + 1:] if '--' in sys.argv else []
    if len(argumentos) < 2:
        print('faltam argumentos: <glb> <saida_prefixo>')
        return 2
    glb, prefixo = argumentos[0], argumentos[1]

    limpar_cena()
    if glb.lower().endswith('.fbx'):
        bpy.ops.import_scene.fbx(filepath=glb)
    else:
        bpy.ops.import_scene.gltf(filepath=glb)

    malhas = [o for o in bpy.data.objects if o.type == 'MESH']
    vertices = sum(len(o.data.vertices) for o in malhas)
    faces = sum(len(o.data.polygons) for o in malhas)
    materiais = sorted({m.name for o in malhas for m in o.data.materials if m})
    imagens = [i.name for i in bpy.data.images if i.size[0] > 0]

    caixa = None
    for o in malhas:
        for canto in o.bound_box:
            ponto = o.matrix_world @ bpy.mathutils.Vector(canto) if hasattr(bpy, 'mathutils') else None
    print('=== INSPECAO ===')
    print(f'arquivo: {os.path.basename(glb)}  ({os.path.getsize(glb) / 1e6:.1f} MB)')
    print(f'objetos de malha: {len(malhas)}')
    print(f'vertices: {vertices}   faces: {faces}')
    print(f'materiais: {materiais}')
    print(f'imagens embutidas: {imagens}')
    if malhas:
        alvo = malhas[0]
        print(f'caixa do 1o objeto (local): {[round(v, 3) for v in alvo.dimensions]}')

    # enquadra a camera automaticamente
    todas = [o for o in bpy.data.objects if o.type == 'MESH']
    if not todas:
        print('sem malha para renderizar')
        return 1
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.view3d.camera_to_view_selected() if bpy.context.area else None
    from mathutils import Vector
    minimo = Vector((1e9, 1e9, 1e9))
    maximo = Vector((-1e9, -1e9, -1e9))
    for o in todas:
        for canto in o.bound_box:
            ponto = o.matrix_world @ Vector(canto)
            minimo = Vector((min(minimo[i], ponto[i]) for i in range(3)))
            maximo = Vector((max(maximo[i], ponto[i]) for i in range(3)))
    centro = (minimo + maximo) / 2
    tamanho = max((maximo - minimo).length, 0.001)

    camera_dados = bpy.data.cameras.new('Camera')
    camera = bpy.data.objects.new('Camera', camera_dados)
    bpy.context.scene.collection.objects.link(camera)
    bpy.context.scene.camera = camera

    luz_dados = bpy.data.lights.new('Luz', type='AREA')
    luz_dados.energy = 2000
    luz = bpy.data.objects.new('Luz', luz_dados)
    luz.location = centro + Vector((tamanho, -tamanho, tamanho))
    bpy.context.scene.collection.objects.link(luz)

    mundo = bpy.data.worlds.new('Mundo')
    mundo.use_nodes = True
    mundo.node_tree.nodes['Background'].inputs[0].default_value = (0.16, 0.16, 0.18, 1)
    bpy.context.scene.world = mundo

    cena = bpy.context.scene
    for motor in ('BLENDER_EEVEE_NEXT', 'BLENDER_EEVEE', 'CYCLES'):
        try:
            cena.render.engine = motor
            break
        except Exception:
            continue
    cena.render.resolution_x = 800
    cena.render.resolution_y = 800
    cena.render.film_transparent = False

    vistas = [('frente', 0.0), ('tres_quartos', 40.0)]
    import math
    for nome, angulo in vistas:
        rad = math.radians(angulo)
        distancia = tamanho * 1.8
        camera.location = centro + Vector((math.sin(rad) * distancia, -math.cos(rad) * distancia, distancia * 0.45))
        direcao = centro - camera.location
        camera.rotation_euler = direcao.to_track_quat('-Z', 'Y').to_euler()
        cena.render.filepath = f'{prefixo}_{nome}.png'
        bpy.ops.render.render(write_still=True)
        print(f'render salvo: {prefixo}_{nome}.png')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
