#!/usr/bin/env python3
"""Prova do modelo corrigido pelo autor: chassi = cerebro-cristal com aura; classe = corpo.

Duas familias de amostra:
  cristal_<chassi>  - o chassi como o autor descreveu: NAO e robo, NAO e humanoide e NAO tem corpo;
                      e um pequeno cerebro de cristal flutuando dentro de uma aura visivel que se
                      comporta como vento gelado (a formacao que cresce ao redor do gelo). A cor e a
                      forma da aura vem do chassi (secao 9b e Body do DataTable).
  corpo_<classe>    - o corpo do JOGADOR, que e da CLASSE: humanizado, com rosto, cabelo e anatomia
                      crivel, no estilo das referencias Art/reference/TronRoll3.jpg e Art/Classes/.

Uso: python3 Tools/comfy/generate_modelo_v001.py [nome]
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

ANCORAGEM = """STYLE ANCHOR - industrial-forge steampunk crossed with action sci-fi. Consistent AAA game art,
physically based rendering, high contrast, restrained palette. Materials: polished brass #c9a227, brushed
brass #a8842c, copper #b87333, oxidized copper #8a4a2a, forged iron #3a3a42, dark steel #2a2a30, rivets
#d8c890, amber glass #ffb040, steam #c8c0a8. Lighting: dusk sky, warm amber key, cold cyan rim, deep
shadows. No text, no watermark. ORIGINAL design - do not imitate any existing franchise."""

CRISTAL = """A RUNNER CHASSIS. It is NOT a robot, NOT a humanoid and NOT a body: it is a small CRYSTAL BRAIN -
a faceted, self-lit crystal core about the size of two fists - floating inside a VISIBLE AURA that behaves like
wind: a luminous formation like the ice-bloom that grows around frozen surfaces, a cold glowing wind circling the
crystal, filaments of light moving as if blown. No arms, no legs, no head, no face, no armour. The crystal
hovers above a small brass plinth. Plain mid-gray background, the aura in motion, cinematic lighting."""

CORPO = """A PLAYER CHARACTER: the body belongs to a CLASS and this is a person, not a machine. HUMANISED:
a human face with visible eyes and expression, hair, believable anatomy, hardware worn OVER a living body,
hands with fingers, synthetic skin at the neck and jaw. No faceless mech head, no armour that replaces the
body. PRESENTATION comes from silhouette and proportion only, never from costume symbolism. STYLE: warm,
hand-crafted game character art in the spirit of a concept sheet, faces readable, colours kept in the anchor palette."""

# (nome, familia, prompt especifico)
AMOSTRAS = [
    ("cristal_cryonix", "cristal", "AURA COLOUR: icy cold blue #5ec8ff (cryogenic, slows everything it touches). AURA FORM: a dense, wide, heavy aura shell wrapping the crystal in thick slow-moving bands of frost."),
    ("cristal_ghostnet", "cristal", "AURA COLOUR: ghost violet #994ce6 (active camouflage). AURA FORM: long thin ribbon-like filaments that trail and curl away like threads of cold wind."),
    ("cristal_overcore", "cristal", "AURA COLOUR: incandescent white #fff0c4 (overcharged, overheats at low health). AURA FORM: a broken, spiking, irregular aura that tears and flickers, spitting unstable light."),
    ("corpo_blaster", "corpo", "CLASS: BLASTER, a ranged specialist of plasma weapons, charged penetrant shot, targeting visor pushed up on the forehead so the face reads, arm cannon on the right forearm. PRESENTATION: FEMININE - hips as wide as the shoulders, marked waist, long neck, soft jaw, large expressive eyes."),
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
    parametros = StdioServerParameters(command=COMFY_MCP, args=[], env={**os.environ, 'COMFY_BIN': COMFY_BIN})

    async with stdio_client(parametros) as (leitura, escrita):
        async with ClientSession(leitura, escrita) as sessao:
            await sessao.initialize()
            for nome, familia, especifico in AMOSTRAS:
                if so_um and nome != so_um:
                    continue
                pasta = Path('Art/generated/cristais') if familia == 'cristal' else Path('Art/generated/corpos')
                pasta.mkdir(parents=True, exist_ok=True)
                destino = pasta / (nome + '_v001.png')
                if destino.exists():
                    print('  ' + nome + ': ja existe, pulando', flush=True)
                    continue

                base = CRISTAL if familia == 'cristal' else CORPO
                prompt = ANCORAGEM + chr(10) + chr(10) + base + chr(10) + chr(10) + especifico

                print('  ' + nome + ' (' + familia + '): gerando...', flush=True)
                try:
                    resposta = await sessao.call_tool('generate_image', {'prompt': prompt, 'wait': True, 'timeout_seconds': 600})
                    conteudo = texto(resposta)
                    prompt_id = id_do_prompt(conteudo)
                    if not prompt_id:
                        print('    falhou: ' + conteudo[:150], flush=True)
                        continue
                    busca = await sessao.call_tool('fetch_outputs', {'prompt_id': prompt_id, 'out_dir': str(pasta.resolve())})
                    arquivos = caminhos(texto(busca))
                    if arquivos and Path(arquivos[0]).exists():
                        shutil.move(arquivos[0], destino)
                        print('    ok -> ' + str(destino), flush=True)
                    else:
                        print('    sem arquivo: ' + texto(busca)[:140], flush=True)
                except Exception as erro:
                    print('    ERRO ' + str(erro), flush=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(asyncio.run(principal()))
