#!/usr/bin/env python3
"""Gera a silhueta dos chassis no angulo do jogo - v002, depois do que a v001 ensinou.

A v001 (ver _contato_chassis_silhueta.png) produziu dez corpos com a MESMA silhueta, a mesma pose e a
camera de frente: cor e material certos, sistema de design errado. A v002 forca, por chassi, o TIPO DE
CORPO do DataTable, uma ASSINATURA de forma unica e uma POSE diferente - e pede camera de cima de
verdade, com o pacote dorsal como leitura principal. Tambem pede a silhueta preta no canto, para o
proprio modelo desenhar pensando em forma.

Uso: python3 Tools/comfy/generate_chassis_v002.py [id_do_chassi]
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
VERSAO = 'v002'

ANCORAGEM = """STYLE ANCHOR - industrial-forge steampunk crossed with action sci-fi. Consistent AAA game art,
physically based rendering, crisp hard-surface detail, high contrast, restrained palette.
Materials: polished brass #c9a227, brushed brass #a8842c, copper #b87333, oxidized copper #8a4a2a,
forged iron #3a3a42, dark steel #2a2a30, rivets #d8c890, amber glass #ffb040, steam #c8c0a8.
Lighting: dusk sky, warm amber key light, cold cyan functional neon (#40e0ff) in seams and vents,
low exposure, deep shadows. No text, no watermark. ORIGINAL design - do not imitate any existing game
franchise, no recognizable mascot silhouette, no copyrighted robot likeness."""

CAMERA = """Camera: looking DOWN at the character from about 55 degrees above the horizon, the top-down action
game view. You see the top of the shoulders, the helmet crest and the whole back unit; the legs are
foreshortened. The dorsal silhouette and the shoulder line are the main read, because that is what the
player sees."""

REGRAS = """Design rules: ONE dominant shape that is unmistakable in a single second, even as a flat black shape;
exaggerated shoulders and helmet crest; big hands and big boots; colour in 60/30/10 (60 per cent neutral
steel/white mass, 30 per cent chassis identity colour, 10 per cent emissive accent); clean panelling with
rivets at the seams; dark outline; flat plain mid-gray background, no cast shadow, no text. In the
top-right corner of the image also draw a small FLAT BLACK SILHOUETTE of the same character on white,
so the shape can be judged at a glance."""

# (id, tipo de corpo do DataTable, assinatura de forma, pose)
CHASSIS = [
    ("charger", "STOCKY - wide, heavy, low centre of gravity, oversized plates", "hunched siege block: ultra-wide shoulder plates, head sunk between them, seismic pistons in the boots", "leaning forward braced, as if about to charge"),
    ("vitaspark", "SLENDER - narrow waist, long thin limbs, aerodynamic", "aerodynamic wedge with swept dorsal fins and elongated skate-like feet", "crouched sprinter start, one hand near the ground"),
    ("aetheric", "SLENDER - narrow, elegant, weightless", "floating core fragments orbiting the torso, long energy conduits trailing like ribbons, open armour showing inner glow", "weightless stance, arms slightly raised, fragments floating around"),
    ("techno", "SLENDER - tall and narrow, analytical", "tall thin sensor-mast head, angular plate shoulders, holographic panels fanned on the forearms", "standing straight, one forearm raised reading a holo panel"),
    ("droneframe", "FRAGILE - small and compact, delicate", "tiny body, one big round optic, large antenna cluster, a small support drone hovering above the shoulder", "small crouched scout stance with the drone hovering above"),
    ("forgekin", "STOCKY - broad, massive, industrial", "broad piston shoulders, chimney stacks and forge vents on the back, welded plates, huge fists", "planted wide with fists at the sides, smoke rising from the stacks"),
    ("ghostnet", "SLENDER - thin and elongated, predatory", "hooded faceless head, elongated thin limbs, refraction panels along the body", "crouched low with long arms reaching down"),
    ("vox", "FRAGILE - slender, human-passing, elegant", "human-like build with an expressive face and a brass resonance grille in the throat, asymmetric speaker cape on one shoulder", "contrapposto with one arm extended as if singing"),
    ("overcore", "UNSTABLE - asymmetric and damaged", "broken asymmetric silhouette: cracked glowing chest, torn plating, venting energy spikes, one oversized arm", "twisted unstable pose with energy venting from the back"),
    ("cryonix", "STOCKY - heavy bulwark, low and wide", "cooling coils wrapped around the limbs, frost-crusted plates, big coolant tanks on the back", "heavy planted stance with cold mist falling from the coils"),
]


def texto(resposta) -> str:
    return chr(10).join(c.text for c in resposta.content if hasattr(c, 'text'))


def id_do_prompt(t: str) -> str:
    achado = re.search(chr(34) + "prompt_id" + chr(34) + r":\s*" + chr(34) + "([0-9a-f-]+)" + chr(34), t)
    if achado:
        return achado.group(1)
    achado = re.search(r'[0-9a-f]{8}-[0-9a-f-]{27,}', t)
    return achado.group(0) if achado else ''


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
            for identificador, corpo, assinatura, pose in CHASSIS:
                if so_um and identificador != so_um:
                    continue
                destino = SAIDA / ('silhueta_' + identificador + '_' + VERSAO + '.png')
                if destino.exists():
                    print('  ' + identificador + ': ja existe, pulando', flush=True)
                    continue

                prompt = (ANCORAGEM + chr(10) + chr(10) + CAMERA
                          + chr(10) + chr(10) + 'BODY TYPE: ' + corpo
                          + chr(10) + 'SHAPE SIGNATURE: ' + assinatura
                          + chr(10) + 'POSE: ' + pose
                          + chr(10) + chr(10) + 'A single full-body sci-fi combat chassis of the PloidrekRPG universe, feet on the ground.'
                          + chr(10) + chr(10) + REGRAS)

                print('  ' + identificador + ': gerando (' + VERSAO + ')...', flush=True)
                try:
                    resposta = await sessao.call_tool('generate_image', {'prompt': prompt, 'wait': True, 'timeout_seconds': 600})
                    conteudo = texto(resposta)
                    prompt_id = id_do_prompt(conteudo)
                    if not prompt_id:
                        print('    falhou: ' + conteudo[:140], flush=True)
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
