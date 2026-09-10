#!/usr/bin/env python3
"""Gera imagem no ComfyUI LOCAL (via comfy-mcp), sem nuvem e sem custo por imagem.

Mesmo caminho que gerou as texturas steampunk de Art/generated/steampunk: servidor MCP
"comfy-mcp" por stdio -> ferramenta generate_image -> fetch_outputs para o disco.

Uso:
    python3 Tools/comfy/generate_image_local.py --prompt-file Art/prompts/vitaspark.md \
        --name chassis_vitaspark_v001 --out-dir Art/generated/chassis

    python3 Tools/comfy/generate_image_local.py --list-tools
"""

from __future__ import annotations

import argparse
import asyncio
import os
import re
import sys
from pathlib import Path

from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

COMFY_MCP = '/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp'
COMFY_BIN = '/Users/volthier/.venvs/comfy-mcp/bin/comfy'


def texto(resposta) -> str:
    return '\n'.join(c.text for c in resposta.content if hasattr(c, 'text'))


def id_do_prompt(texto_resposta: str) -> str:
    achado = re.search(r'"prompt_id":\s*"([0-9a-f-]+)"', texto_resposta)
    if achado:
        return achado.group(1)
    achado = re.search(r'[0-9a-f]{8}-[0-9a-f-]{27,}', texto_resposta)
    return achado.group(0) if achado else ''


async def principal() -> int:
    ap = argparse.ArgumentParser(description='Gera imagem no ComfyUI local (comfy-mcp).')
    ap.add_argument('--prompt')
    ap.add_argument('--prompt-file')
    ap.add_argument('--name', help='nome base do arquivo de saida')
    ap.add_argument('--out-dir', help='pasta de saida (criada se nao existir)')
    ap.add_argument('--timeout', type=int, default=600)
    ap.add_argument('--list-tools', action='store_true')
    args = ap.parse_args()

    if not Path(COMFY_MCP).exists():
        print(f'nao achei o servidor comfy-mcp em {COMFY_MCP}', file=sys.stderr)
        return 2

    parametros = StdioServerParameters(command=COMFY_MCP, args=[],
                                       env={**os.environ, 'COMFY_BIN': COMFY_BIN})

    async with stdio_client(parametros) as (leitura, escrita):
        async with ClientSession(leitura, escrita) as sessao:
            await sessao.initialize()

            if args.list_tools:
                ferramentas = await sessao.list_tools()
                for f in ferramentas.tools:
                    # o SDK expoe input_schema (pydantic); versoes antigas usavam inputSchema
                    esquema = getattr(f, 'input_schema', None) or getattr(f, 'inputSchema', None) or {}
                    campos = list((esquema or {}).get('properties', {}).keys())
                    print(f'  - {f.name}: {campos}')
                return 0

            if not (args.prompt or args.prompt_file):
                print('preciso de --prompt ou --prompt-file', file=sys.stderr)
                return 2

            prompt = args.prompt if args.prompt else Path(args.prompt_file).read_text(encoding='utf-8')
            destino = str(Path(args.out_dir or '.').resolve())
            Path(destino).mkdir(parents=True, exist_ok=True)

            print(f'gerando no ComfyUI local ({args.name or "saida"})...', flush=True)
            resposta = await sessao.call_tool('generate_image',
                                               {'prompt': prompt, 'wait': True, 'timeout_seconds': args.timeout})
            conteudo = texto(resposta)
            prompt_id = id_do_prompt(conteudo)
            print('  prompt_id:', prompt_id or '(sem id)', flush=True)
            print('  ', conteudo[:200].replace('\n', ' '), flush=True)

            if prompt_id:
                busca = await sessao.call_tool('fetch_outputs', {'prompt_id': prompt_id, 'out_dir': destino})
                print('  arquivos:', texto(busca)[:400].replace('\n', ' '), flush=True)
            return 0


if __name__ == '__main__':
    raise SystemExit(asyncio.run(principal()))
