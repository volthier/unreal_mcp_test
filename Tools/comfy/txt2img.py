#!/usr/bin/env python3
'''Gera imagem no ComfyUI local pela API HTTP direta, com RESOLUCAO propria.

Por que existe: o generate_image do comfy-mcp usa um template fixo de 1024x1024. Para cenario (16:9) e
para arte de tela de login isso nao serve. Aqui o tamanho, o passo e a semente sao meus.

Uso: python3 Tools/comfy/txt2img.py --prompt "..." --out Art/generated/cenario/x.png [--largura 1280 --altura 720]
'''

from __future__ import annotations

import argparse
import copy
import json
import sys
import time
import urllib.parse
import urllib.request
from pathlib import Path

COMFY = 'http://127.0.0.1:8188'
BASE = Path('Tools/comfy/workflows/z_image_turbo_base.json')


def get_json(caminho):
    with urllib.request.urlopen(COMFY + caminho, timeout=60) as r:
        return json.loads(r.read().decode('utf-8'))


def post_json(caminho, dados):
    pedido = urllib.request.Request(COMFY + caminho, data=json.dumps(dados).encode('utf-8'),
                                    headers={'Content-Type': 'application/json'})
    with urllib.request.urlopen(pedido, timeout=120) as r:
        return json.loads(r.read().decode('utf-8'))


def principal():
    ap = argparse.ArgumentParser()
    ap.add_argument('--prompt', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--largura', type=int, default=1280)
    ap.add_argument('--altura', type=int, default=720)
    ap.add_argument('--passos', type=int, default=8)
    ap.add_argument('--semente', type=int, default=0)
    args = ap.parse_args()

    grafo = copy.deepcopy(json.loads(BASE.read_text(encoding='utf-8')))
    for no in grafo.values():
        if no['class_type'] == 'EmptySD3LatentImage':
            no['inputs']['width'] = args.largura
            no['inputs']['height'] = args.altura
        if no['class_type'] == 'CLIPTextEncode':
            no['inputs']['text'] = args.prompt
        if no['class_type'] == 'KSampler':
            no['inputs']['steps'] = args.passos
            no['inputs']['seed'] = args.semente

    resposta = post_json('/prompt', {'prompt': grafo})
    prompt_id = resposta.get('prompt_id')
    if not prompt_id:
        print('falhou ao enfileirar:', json.dumps(resposta)[:300], file=sys.stderr)
        return 1
    print('  na fila:', prompt_id, flush=True)

    for _ in range(300):
        time.sleep(2)
        historico = get_json('/history/' + prompt_id)
        if prompt_id in historico:
            for valor in historico[prompt_id].get('outputs', {}).values():
                for imagem in valor.get('images', []):
                    destino = Path(args.out)
                    destino.parent.mkdir(parents=True, exist_ok=True)
                    consulta = ('/view?filename=' + urllib.parse.quote(imagem['filename'])
                                + '&type=output&subfolder=' + urllib.parse.quote(imagem.get('subfolder', '')))
                    with urllib.request.urlopen(COMFY + consulta, timeout=180) as r:
                        destino.write_bytes(r.read())
                    print('  pronto ->', destino, flush=True)
                    return 0
            print('  saida sem imagem', file=sys.stderr)
            return 1
    print('  tempo esgotado', file=sys.stderr)
    return 1


if __name__ == '__main__':
    raise SystemExit(principal())
