#!/usr/bin/env python3
"""Gera uma imagem a partir de uma REFERENCIA (img2img) no ComfyUI local - o estilo vem da imagem.

Por que existe: descrever estilo em palavras nunca cola. O jeito de os corpos das classes ficarem no
estilo das referencias do projeto (Art/reference/TronRoll3.jpg, Art/Classes/sample_*.jpg) e usar a
propria imagem como base e deixar o modelo reinterpretar com denoise parcial.

Como funciona:
  1. le o grafo base que o ComfyUI executou (Tools/comfy/workflows/z_image_turbo_base.json);
  2. troca o no de latent vazio por LoadImage + VAEEncode (a imagem vira o ponto de partida);
  3. sobe a referencia para o ComfyUI, enfileira, espera e baixa o resultado.

Sem nuvem e sem custo: e o ComfyUI da maquina, chamado direto pela API HTTP dele.

Uso:
  python3 Tools/comfy/img2img_style.py --ref Art/Classes/sample_tank.jpg \\
      --prompt "..." --out Art/generated/corpos/corpo_breaker_v001.png [--denoise 0.62]
"""

from __future__ import annotations

import argparse
import copy
import json
import mimetypes
import sys
import time
import urllib.request
import uuid
from pathlib import Path

COMFY = 'http://127.0.0.1:8188'
BASE = Path('Tools/comfy/workflows/z_image_turbo_base.json')


def post_json(caminho: str, dados: dict) -> dict:
    corpo = json.dumps(dados).encode('utf-8')
    pedido = urllib.request.Request(COMFY + caminho, data=corpo, headers={'Content-Type': 'application/json'})
    with urllib.request.urlopen(pedido, timeout=60) as resposta:
        return json.loads(resposta.read().decode('utf-8'))


def get_json(caminho: str) -> dict:
    with urllib.request.urlopen(COMFY + caminho, timeout=60) as resposta:
        return json.loads(resposta.read().decode('utf-8'))


def subir_imagem(caminho: Path) -> str:
    """Multipart manual: sem dependencia externa."""
    limite = '----dsh' + uuid.uuid4().hex
    tipo = mimetypes.guess_type(caminho.name)[0] or 'image/png'
    dados = caminho.read_bytes()
    partes = []
    partes.append(('--' + limite + '\r\n').encode())
    partes.append(('Content-Disposition: form-data; name="image"; filename="' + caminho.name + '"\r\n').encode())
    partes.append(('Content-Type: ' + tipo + '\r\n\r\n').encode())
    partes.append(dados)
    partes.append(('\r\n--' + limite + '\r\n').encode())
    partes.append(b'Content-Disposition: form-data; name="overwrite"\r\n\r\ntrue\r\n')
    partes.append(('--' + limite + '--\r\n').encode())
    corpo = b''.join(partes)
    pedido = urllib.request.Request(COMFY + '/upload/image', data=corpo,
                                    headers={'Content-Type': 'multipart/form-data; boundary=' + limite})
    with urllib.request.urlopen(pedido, timeout=120) as resposta:
        return json.loads(resposta.read().decode('utf-8')).get('name', caminho.name)


def montar_grafo(prompt: str, imagem: str, denoise: float, semente: int, largura: int, altura: int) -> dict:
    grafo = json.loads(BASE.read_text(encoding='utf-8'))
    grafo = copy.deepcopy(grafo)

    # acha os nos que interessam pelo tipo, nao por id (o id muda entre execucoes)
    id_do_vazio = next((i for i, n in grafo.items() if n['class_type'] == 'EmptySD3LatentImage'), None)
    id_do_ks = next((i for i, n in grafo.items() if n['class_type'] == 'KSampler'), None)
    id_do_texto = next((i for i, n in grafo.items() if n['class_type'] == 'CLIPTextEncode'), None)
    id_do_vae = next((i for i, n in grafo.items() if n['class_type'] == 'VAELoader'), None)
    if not all([id_do_vazio, id_do_ks, id_do_texto, id_do_vae]):
        raise SystemExit('grafo base sem os nos esperados')

    # troca o latent vazio por imagem -> VAEEncode
    grafo['img_load'] = {'class_type': 'LoadImage', 'inputs': {'image': imagem, 'upload': 'image'}}
    grafo['img_encode'] = {'class_type': 'VAEEncode',
                           'inputs': {'pixels': ['img_load', 0], 'vae': [id_do_vae, 0]}}
    grafo.pop(id_do_vazio)
    grafo[id_do_ks]['inputs']['latent_image'] = ['img_encode', 0]
    grafo[id_do_ks]['inputs']['denoise'] = denoise
    grafo[id_do_ks]['inputs']['seed'] = semente
    grafo[id_do_texto]['inputs']['text'] = prompt

    # a resolucao passa a vir da imagem, mas mantemos os campos por seguranca
    return grafo


def principal() -> int:
    ap = argparse.ArgumentParser(description='img2img no ComfyUI local, com referencia de estilo.')
    ap.add_argument('--ref', required=True, help='imagem de referencia (base do estilo)')
    ap.add_argument('--prompt', required=True)
    ap.add_argument('--out', required=True)
    ap.add_argument('--denoise', type=float, default=0.62, help='0.4 cola na referencia, 0.8 solta')
    ap.add_argument('--seed', type=int, default=0)
    args = ap.parse_args()

    if not BASE.exists():
        print('falta o grafo base:', BASE, file=sys.stderr)
        return 2

    referencia = Path(args.ref)
    if not referencia.exists():
        print('nao achei a referencia:', referencia, file=sys.stderr)
        return 2

    nome_remoto = subir_imagem(referencia)
    print('  referencia enviada:', nome_remoto, flush=True)

    grafo = montar_grafo(args.prompt, nome_remoto, args.denoise, args.seed, 0, 0)
    resposta = post_json('/prompt', {'prompt': grafo})
    prompt_id = resposta.get('prompt_id')
    if not prompt_id:
        print('  falha ao enfileirar:', json.dumps(resposta)[:200], file=sys.stderr)
        return 1
    print('  na fila:', prompt_id, flush=True)

    for _ in range(180):
        time.sleep(2)
        historico = get_json('/history/' + prompt_id)
        if prompt_id in historico:
            saidas = historico[prompt_id].get('outputs', {})
            for valor in saidas.values():
                for img in valor.get('images', []):
                    destino = Path(args.out)
                    destino.parent.mkdir(parents=True, exist_ok=True)
                    consulta = '/view?filename=' + urllib.parse.quote(img['filename']) + '&type=' + img.get('type', 'output') + '&subfolder=' + urllib.parse.quote(img.get('subfolder', ''))
                    with urllib.request.urlopen(COMFY + consulta, timeout=120) as r:
                        destino.write_bytes(r.read())
                    print('  pronto ->', destino, flush=True)
                    return 0
            print('  sem imagem na saida', file=sys.stderr)
            return 1
    print('  tempo esgotado esperando o ComfyUI', file=sys.stderr)
    return 1


if __name__ == '__main__':
    import urllib.parse  # usado no /view
    raise SystemExit(principal())
