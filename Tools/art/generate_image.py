#!/usr/bin/env python3
"""Gera imagem por prompt usando as chaves de Tools/api_tokens.json (nunca versionado).

Existe para o ciclo de arte do PloidrekRPG: prompt do Docs/Prompts_Arte_Ploidrek.md -> imagem ->
eu OLHO o arquivo -> reprovado volta com o motivo escrito, aprovado segue para import no engine.

Uso:
    python3 Tools/art/generate_image.py --prompt-file Art/prompts/vitaspark.md \
        --out Art/generated/chassis/vitaspark_v001.png [--provider gemini|openai] [--model ...]

    python3 Tools/art/generate_image.py --prompt "..." --out x.png --aspect 3:4

A chave e lida do arquivo e passada direto para a API: ela NUNCA e impressa nem gravada em log.
"""

from __future__ import annotations

import argparse
import base64
import json
import re
import subprocess
import sys
from pathlib import Path

RAIZ = Path(__file__).resolve().parents[2]
TOKENS = RAIZ / 'Tools' / 'api_tokens.json'

PADRAO_POR_PROVEDOR = {
    'gemini': 'gemini-3-pro-image',
    'chatgpt-service': 'gpt-image-1.5',
}


def ler_chaves() -> dict[str, str]:
    """Le o arquivo de tokens. Ele NAO e JSON estrito (chaves sem aspas) — por isso o parse e tolerante."""
    if not TOKENS.exists():
        raise SystemExit(f'nao achei {TOKENS}')
    chaves: dict[str, str] = {}
    provedor = None
    for linha in TOKENS.read_text(encoding='utf-8').splitlines():
        achado = re.search(r'provider\s*:\s*"?([\w\-]+)"?', linha)
        if achado:
            provedor = achado.group(1)
        achado = re.search(r'key\s*:\s*"?([^",\s}]+)"?', linha)
        if achado and provedor:
            chaves[provedor] = achado.group(1)
            provedor = None
    return chaves


def gerar_gemini(chave: str, modelo: str, prompt: str, destino: Path, aspecto: str) -> tuple[bool, str]:
    corpo = {
        'contents': [{'parts': [{'text': prompt}]}],
        'generationConfig': {'responseModalities': ['IMAGE'], 'imageConfig': {'aspectRatio': aspecto}},
    }
    url = f'https://generativelanguage.googleapis.com/v1beta/models/{modelo}:generateContent?key={chave}'
    r = subprocess.run(['curl', '-s', '-m', '300', url,
                        '-H', 'Content-Type: application/json', '-d', json.dumps(corpo)],
                       capture_output=True, text=True)
    try:
        resposta = json.loads(r.stdout)
    except Exception:
        return False, 'resposta nao-JSON: ' + r.stdout[:200]
    if 'error' in resposta:
        return False, str(resposta['error'].get('message', resposta['error']))[:200]

    for candidato in resposta.get('candidates', []):
        for parte in candidato.get('content', {}).get('parts', []):
            dados = parte.get('inlineData') or parte.get('inline_data')
            if dados and dados.get('data'):
                destino.parent.mkdir(parents=True, exist_ok=True)
                destino.write_bytes(base64.b64decode(dados['data']))
                return True, f'imagem salva ({destino.stat().st_size // 1024} KB, mime {dados.get("mimeType", "?")})'
    return False, 'resposta sem imagem: ' + json.dumps(resposta)[:200]


def gerar_openai(chave: str, modelo: str, prompt: str, destino: Path, tamanho: str) -> tuple[bool, str]:
    corpo = {'model': modelo, 'prompt': prompt, 'size': tamanho, 'n': 1}
    if modelo.startswith('gpt-image'):
        corpo['quality'] = 'high'
    r = subprocess.run(['curl', '-s', '-m', '300', 'https://api.openai.com/v1/images/generations',
                        '-H', 'Content-Type: application/json', '-H', f'Authorization: Bearer {chave}',
                        '-d', json.dumps(corpo)], capture_output=True, text=True)
    try:
        resposta = json.loads(r.stdout)
    except Exception:
        return False, 'resposta nao-JSON: ' + r.stdout[:200]
    if 'error' in resposta:
        return False, str(resposta['error'].get('message', resposta['error']))[:200]

    for item in resposta.get('data', []):
        if item.get('b64_json'):
            destino.parent.mkdir(parents=True, exist_ok=True)
            destino.write_bytes(base64.b64decode(item['b64_json']))
            return True, f'imagem salva ({destino.stat().st_size // 1024} KB)'
    return False, 'resposta sem imagem: ' + json.dumps(resposta)[:200]


def principal() -> int:
    ap = argparse.ArgumentParser(description='Gera imagem por prompt (arte do PloidrekRPG).')
    grupo = ap.add_mutually_exclusive_group(required=True)
    grupo.add_argument('--prompt')
    grupo.add_argument('--prompt-file')
    ap.add_argument('--out', required=True)
    ap.add_argument('--provider', default='gemini', choices=['gemini', 'chatgpt-service'])
    ap.add_argument('--model', default=None)
    ap.add_argument('--aspect', default='3:4', help='gemini: 1:1, 3:4, 4:3, 9:16, 16:9')
    ap.add_argument('--size', default='1024x1536', help='openai: 1024x1024, 1024x1536, 1536x1024')
    args = ap.parse_args()

    prompt = args.prompt if args.prompt else Path(args.prompt_file).read_text(encoding='utf-8')
    modelo = args.model or PADRAO_POR_PROVEDOR[args.provider]
    destino = Path(args.out)

    chaves = ler_chaves()
    chave = chaves.get(args.provider)
    if not chave:
        print(f'sem chave para o provedor {args.provider}', file=sys.stderr)
        return 2

    print(f'gerando com {args.provider}/{modelo} -> {destino}')
    if args.provider == 'gemini':
        ok, mensagem = gerar_gemini(chave, modelo, prompt, destino, args.aspect)
    else:
        ok, mensagem = gerar_openai(chave, modelo, prompt, destino, args.size)

    print(('OK: ' if ok else 'FALHOU: ') + mensagem)
    return 0 if ok else 1


if __name__ == '__main__':
    raise SystemExit(principal())
