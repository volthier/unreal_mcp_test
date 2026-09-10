#!/usr/bin/env python3
"""v003 - chassis HUMANIZADOS e com espectro de apresentacao (adendo do autor).

A v002 acertou a forma (silhueta distinta), mas o autor apontou o que faltava: os chassis estavam
robotizados demais - sem corpo humano, sem rosto, sem genero. A v003 poe gente dentro do hardware:
rosto humano com expressao, anatomia sob as placas, cabelo/cranio proprio, e a apresentacao
(Masculine / Feminine / MaleFem / Femasc / Androgynous) comunicada por SILHUETA e PROPORCAO.

Uso: python3 Tools/comfy/generate_chassis_v003.py [id_do_chassi]
"""

from __future__ import annotations

import asyncio
import json
import os
import re
import shutil
import sys
from pathlib import Path

from mcp import ClientSession, StdioServerParameters
from mcp.client.stdio import stdio_client

COMFY_MCP = '/Users/volthier/.venvs/comfy-mcp/bin/comfy-mcp'
COMFY_BIN = '/Users/volthier/.venvs/comfy-mcp/bin/comfy'
SAIDA = Path('Art/generated/chassis').resolve()
VERSAO = 'v003'

ANCORAGEM = """STYLE ANCHOR - industrial-forge steampunk crossed with action sci-fi. Consistent AAA game art,
physically based rendering, crisp hard-surface detail, high contrast, restrained palette.
Materials: polished brass #c9a227, brushed brass #a8842c, copper #b87333, oxidized copper #8a4a2a,
forged iron #3a3a42, dark steel #2a2a30, rivets #d8c890, steam #c8c0a8. Functional neon and the emissive
core use the chassis identity colour. Lighting: dusk sky, warm amber key, cold cyan rim, deep shadows.
No text, no watermark. ORIGINAL design - do not imitate any existing franchise."""

HUMANIZADO = """HUMANISED RULES - these are human beings wearing advanced hardware, not robots: a human face with
visible eyes and expression (a visor is an accessory, never a substitute for a face); human anatomy under
the plating - neck, waist, hip and shoulder articulation, hands with fingers; the hardware is worn OVER a
living body, so synthetic skin may show at the neck, jaw, throat and hands; each chassis has its own hair
or skull shape, because that is the fastest human read. No faceless mech head, no armour that replaces the
body."""

LEITURA = """Readability: one dominant shape that is unmistakable in a single second; colour in 60/30/10; clean
panelling with rivets at the seams; flat plain mid-gray background, no cast shadow, no text. In the top-right
corner also draw a small FLAT BLACK SILHOUETTE of the same character on white."""

# (id, apresentacao, descricao do corpo/rosto, regra da apresentacao)
CHASSIS = [
    ("vox", "FEMININE", "slender fragile human-passing build, hips as wide as the shoulders, long neck, marked waist, soft jaw and large expressive eyes", "PRESENTATION RULES: silhouette and proportion carry the presentation, never costume symbolism"),
    ("forgekin", "MASCULINE", "broad masculine build with shoulders clearly wider than the hips, V-shaped torso, thick neck, heavy jaw and strong brow", "PRESENTATION RULES: silhouette and proportion carry the presentation, never costume symbolism"),
    ("vitaspark", "ANDROGYNOUS", "balanced androgynous build, shoulders the same width as the hips, no binary marker, neutral delicate features, ambiguous on purpose", "PRESENTATION RULES: silhouette and proportion carry the presentation, never costume symbolism"),
]


def texto(resposta) -> str:
    return chr(10).join(c.text for c in resposta.content if hasattr(c, 'text'))


def id_do_prompt(t: str) -> str:
    achado = re.search('prompt_id[^0-9a-f]*([0-9a-f-]{8,})', t)
    return achado.group(1) if achado else ''


def caminhos(t: str) -> list:
    try:
        dados = json.loads(t)
    except json.JSONDecodeError:
        return []
    return [f['path'] for f in dados.get('files', []) if isinstance(f, dict) and f.get('path')]


async def principal() -> int:
    so_um = sys.argv[1] if len(sys.argv) > 1 else None
    SAIDA.mkdir(parents=True, exist_ok=True)
    parametros = StdioServerParameters(command=COMFY_MCP, args=[], env={**os.environ, 'COMFY_BIN': COMFY_BIN})

    async with stdio_client(parametros) as (leitura, escrita):
        async with ClientSession(leitura, escrita) as sessao:
            await sessao.initialize()
            for identificador, apresentacao, corpo, regra in CHASSIS:
                if so_um and identificador != so_um:
                    continue
                destino = SAIDA / ('humanizado_' + identificador + '_' + VERSAO + '.png')
                if destino.exists():
                    print('  ' + identificador + ': ja existe, pulando', flush=True)
                    continue

                prompt = (ANCORAGEM + chr(10) + chr(10) + HUMANIZADO + chr(10) + chr(10)
                          + 'PRESENTATION: ' + apresentacao + chr(10) + 'BODY: ' + corpo + chr(10) + regra
                          + chr(10) + chr(10) + 'Full body, standing, seen from the front, plain background.'
                          + chr(10) + chr(10) + LEITURA)

                print('  ' + identificador + ' (' + apresentacao + '): gerando...', flush=True)
                try:
                    resposta = await sessao.call_tool('generate_image', {'prompt': prompt, 'wait': True, 'timeout_seconds': 600})
                    conteudo = texto(resposta)
                    prompt_id = id_do_prompt(conteudo)
                    if not prompt_id:
                        print('    falhou: ' + conteudo[:160], flush=True)
                        continue
                    busca = await sessao.call_tool('fetch_outputs', {'prompt_id': prompt_id, 'out_dir': str(SAIDA)})
                    arquivos = caminhos(texto(busca))
                    if arquivos and Path(arquivos[0]).exists():
                        shutil.move(arquivos[0], destino)
                        print('    ok -> ' + destino.name, flush=True)
                    else:
                        print('    sem arquivo: ' + texto(busca)[:140], flush=True)
                except Exception as erro:
                    print('    ERRO ' + str(erro), flush=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(asyncio.run(principal()))
