#!/usr/bin/env python3
"""Gera a familia dos APAGADOS - as cascas tomadas pela nevoa (GDD 2.6, decisao do autor).

A arte robotica que sobrou dos chassis (Art/generated/apagados, v001 e v002) virou o conceito desta
familia. Este script expande a familia com a regra certa: casca oca, sem rosto, luz de identidade
FALHANDO, ferrugem e pecas trocadas entre chassis, nevoa saindo das juntas, silhueta humana quebrada.

Uso: python3 Tools/comfy/generate_apagados_v001.py [nome]
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
SAIDA = Path('Art/generated/apagados').resolve()

ANCORAGEM = """STYLE ANCHOR - industrial-forge steampunk crossed with action sci-fi. Consistent AAA game art,
physically based rendering, high contrast, restrained palette. Materials: polished brass #c9a227, brushed
brass #a8842c, copper #b87333, oxidized copper #8a4a2a, forged iron #3a3a42, dark steel #2a2a30, rivets
#d8c890, amber glass #ffb040, steam #c8c0a8. Lighting: dusk sky, amber key, cold cyan rim, deep shadows.
No text, no watermark. ORIGINAL design - do not imitate any existing franchise."""

APAGADO = """THE APAGADOS - canon: the taken by the mist; what is left of them stays inside, and sometimes
comes out. They are HOLLOW SHELLS of humanoid combat chassis, not living people: breaches in the plating show
an EMPTY interior with loose cables and nothing inside; no face - a blank face plate or a dead black visor, at
most ONE optic flickering; the identity core of the original chassis is still there but FAILING - flickering,
half dark, bleeding light through the cracks; heavy oxidation and mismatched scavenged parts from other chassis
(a Charger arm on a Vox torso); mist seeping out of every seam; the human silhouette is BROKEN - one oversized
arm, a tilted head, a dragging leg. The read must be: human, but not."""

LEITURA = """Readability: one dominant shape that reads in a single second; flat plain mid-gray background, no cast
shadow, no text."""

# (nome, categoria do GDD 7.2, descricao da unidade)
APAGADOS = [
    ("casca", "Regular", "a human-sized hollow shell of a combat chassis, hollow torso breach, dead visor, failing core, mismatched arm"),
    ("arrastado", "Regular", "a shell that drags one leg, torso twisted, plating torn open at the chest showing empty interior, mist from the seams"),
    ("optico", "Minion", "a small compact shell unit, single flickering optic, bent antenna, nothing else on the face"),
    ("remendo", "Regular", "a shell rebuilt from the parts of four different chassis, welded seams, oxidised, core half dark"),
    ("gigante", "Elite", "an overgrown broken shell, an extra limb welded to the back, two failed cores flickering out of sync, mist pouring out"),
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
            for nome, categoria, descricao in APAGADOS:
                if so_um and nome != so_um:
                    continue
                destino = SAIDA / ('creature_apagado_' + nome + '_v001.png')
                if destino.exists():
                    print('  ' + nome + ': ja existe, pulando', flush=True)
                    continue

                prompt = (ANCORAGEM + chr(10) + chr(10) + APAGADO + chr(10) + chr(10)
                          + 'ENEMY CATEGORY: ' + categoria + chr(10) + 'UNIT: ' + descricao
                          + chr(10) + chr(10) + 'Full body, standing, seen from the front, plain background.'
                          + chr(10) + chr(10) + LEITURA)

                print('  ' + nome + ' (' + categoria + '): gerando...', flush=True)
                try:
                    resposta = await sessao.call_tool('generate_image', {'prompt': prompt, 'wait': True, 'timeout_seconds': 600})
                    conteudo = texto(resposta)
                    prompt_id = id_do_prompt(conteudo)
                    if not prompt_id:
                        print('    falhou: ' + conteudo[:150], flush=True)
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
