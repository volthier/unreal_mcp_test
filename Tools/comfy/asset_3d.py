#!/usr/bin/env python3
'''Imagem -> malha 3D pelo ComfyUI LOCAL, falando direto com a API HTTP dele.

Dois aprendizados que criaram este script:
1. IMAGE-TO-3D NAO ADIVINHA. A primeira tentativa reconstruiu a CENA (chao, paineis, diorama) porque a
   imagem de entrada tinha pedestal, casco e fundo. A entrada certa e UM objeto, fundo branco puro, sem
   chao, sem props, sem efeito. E efeito translucido (aura, brilho, nevoa) NAO pode virar malha - e VFX.
2. NAO USAR O comfy-mcp PARA workflow. Ele quebrou (ComfyCliError) e caiu ate com o arquivo que havia
   funcionado minutos antes. A API HTTP do ComfyUI e estavel: /upload/image, /prompt, /history, /view.

Uso:
  python3 Tools/comfy/asset_3d.py --imagem Art/generated/cristais/xxx.png --nome cristal_cryonix
'''

from __future__ import annotations

import argparse
import copy
import json
import mimetypes
import subprocess
import sys
import time
import urllib.parse
import urllib.request
import uuid
from pathlib import Path

COMFY = 'http://127.0.0.1:8188'
MODELO = Path('Tools/comfy/workflows/cristal_3d.json')
PASTA_3D = Path('Art/generated/3d')
BLENDER = '/Applications/Blender.app/Contents/MacOS/Blender'


def get_json(caminho):
    with urllib.request.urlopen(COMFY + caminho, timeout=60) as resposta:
        return json.loads(resposta.read().decode('utf-8'))


def post_json(caminho, dados):
    pedido = urllib.request.Request(COMFY + caminho, data=json.dumps(dados).encode('utf-8'),
                                    headers={'Content-Type': 'application/json'})
    with urllib.request.urlopen(pedido, timeout=120) as resposta:
        return json.loads(resposta.read().decode('utf-8'))


def subir_imagem(caminho):
    limite = '----dsh' + uuid.uuid4().hex
    tipo = mimetypes.guess_type(caminho.name)[0] or 'image/png'
    CR = chr(13) + chr(10)
    partes = [
        ('--' + limite + CR).encode(),
        ('Content-Disposition: form-data; name="image"; filename="' + caminho.name + '"' + CR).encode(),
        ('Content-Type: ' + tipo + CR + CR).encode(),
        caminho.read_bytes(),
        (CR + '--' + limite + CR).encode(),
        ('Content-Disposition: form-data; name="overwrite"' + CR + CR + 'true' + CR).encode(),
        ('--' + limite + '--' + CR).encode(),
    ]
    pedido = urllib.request.Request(COMFY + '/upload/image', data=b''.join(partes),
                                    headers={'Content-Type': 'multipart/form-data; boundary=' + limite})
    with urllib.request.urlopen(pedido, timeout=180) as resposta:
        return json.loads(resposta.read().decode('utf-8')).get('name', caminho.name)


def baixar(nome, subpasta, destino):
    consulta = '/view?filename=' + urllib.parse.quote(nome) + '&type=output&subfolder=' + urllib.parse.quote(subpasta or '')
    with urllib.request.urlopen(COMFY + consulta, timeout=300) as resposta:
        destino.parent.mkdir(parents=True, exist_ok=True)
        destino.write_bytes(resposta.read())


def principal():
    ap = argparse.ArgumentParser(description='imagem -> malha 3D no ComfyUI local (API HTTP direta).')
    ap.add_argument('--imagem', required=True)
    ap.add_argument('--nome', required=True)
    ap.add_argument('--sem-render', action='store_true')
    args = ap.parse_args()

    imagem = Path(args.imagem)
    if not imagem.exists() or not MODELO.exists():
        print('faltando imagem ou modelo de workflow', file=sys.stderr)
        return 2

    print('  1/3 enviando a imagem para o ComfyUI...', flush=True)
    nome_remoto = subir_imagem(imagem)
    print('      nome no ComfyUI:', nome_remoto, flush=True)

    grafo = copy.deepcopy(json.loads(MODELO.read_text(encoding='utf-8')))
    for no in grafo.values():
        if no['class_type'] == 'LoadImage':
            no['inputs']['image'] = nome_remoto
        if no['class_type'] == 'SaveGLB':
            no['inputs']['filename_prefix'] = args.nome

    print('  2/3 enfileirando o Hunyuan3D...', flush=True)
    resposta = post_json('/prompt', {'prompt': grafo})
    prompt_id = resposta.get('prompt_id')
    if not prompt_id:
        print('      falhou:', json.dumps(resposta)[:300], file=sys.stderr)
        return 1
    print('      na fila:', prompt_id, flush=True)

    for _ in range(400):
        time.sleep(3)
        historico = get_json('/history/' + prompt_id)
        if prompt_id in historico:
            saidas = historico[prompt_id].get('outputs', {})
            for valor in saidas.values():
                for item in valor.get('images', []) + valor.get('3d', []) + valor.get('gltf', []):
                    destino = PASTA_3D / (args.nome + '_v001.glb')
                    baixar(item['filename'], item.get('subfolder', ''), destino)
                    print('      malha:', destino, str(round(destino.stat().st_size / 1e6, 1)) + ' MB', flush=True)
                    if not args.sem_render:
                        print('  3/3 renderizando para conferir...', flush=True)
                        subprocess.run([BLENDER, '--background', '--python', 'Tools/Blender/inspect_and_render.py',
                                        '--', str(destino.resolve()), str((PASTA_3D / args.nome).resolve())],
                                       capture_output=True, text=True)
                    return 0
            print('      saida sem malha:', json.dumps(saidas)[:200], file=sys.stderr)
            return 1
    print('      tempo esgotado', file=sys.stderr)
    return 1


if __name__ == '__main__':
    raise SystemExit(principal())
