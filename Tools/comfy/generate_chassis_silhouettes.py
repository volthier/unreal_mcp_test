#!/usr/bin/env python3
"""Gera a SILHUETA dos 10 chassis no angulo em que o jogador os ve (camera fixa alta, top-down 3D).

Por que silhueta primeiro: com camera alta e distante o que informa o personagem e silhueta e cor -
detalhe fino de superficie quase nao aparece (Docs/Modelo_Interacao.md).

Usa o comfy-mcp (ComfyUI local, z_image_turbo): caminho gratuito, sem nuvem.
Uso: python3 Tools/comfy/generate_chassis_silhouettes.py [id_do_chassi]
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

ANCORAGEM = """STYLE ANCHOR - industrial-forge steampunk crossed with action sci-fi. Consistent AAA game art,
physically based rendering, crisp hard-surface detail, high contrast, restrained palette.
Materials: polished brass #c9a227, brushed brass #a8842c, copper #b87333, oxidized copper #8a4a2a,
forged iron #3a3a42, dark steel #2a2a30, rivets #d8c890, amber glass #ffb040, steam #c8c0a8.
Lighting: dusk sky, warm amber key light, cold cyan functional neon (#40e0ff) in seams and vents,
low exposure, deep shadows. No text, no watermark. ORIGINAL design - do not imitate any existing
game franchise, no recognizable mascot silhouette, no copyrighted robot likeness."""

REGRAS = """Readability rules for a top-down game: ONE strong dominant shape that reads in a single second
even as a black silhouette; shoulders, helmet crest and dorsal elements emphasized because they are what
the player sees from above; big hands and big boots; colour in 60/30/10 (60% neutral steel/white mass,
30% chassis identity colour, 10% emissive accent); clean panelling with rivets at the seams; dark
outline, flat plain mid-gray background, no cast shadow on the floor, no text."""

CHASSIS = [
  ("charger", "Stocky siege chassis built for impact: thick overlapping plates, heavy hydraulic servos, b..."),
  ("vitaspark", "Slender speed chassis: light hips, tall dorsal heat dissipators, swept aerodynamic fins on..."),
  ("aetheric", "Slender chassis bonded to cosmic energy: floating core fragments orbiting the torso, glowi..."),
  ("techno", "Slender analytical chassis: brain-core head with a sensor array crest, data conduits acros..."),
  ("droneframe", "Small fragile recon chassis: compact low frame, one large round optic, antenna cluster on ..."),
  ("forgekin", "Stocky living-forge tank: hydraulic piston arms, fusion-welded armour plates, industrial f..."),
  ("ghostnet", "Slender semi-optical infiltration chassis: camouflage refraction panels, no visible face, ..."),
  ("vox", "Fragile human-passing chassis with an expressive face and vocal resonance emitters in the ..."),
  ("overcore", "Unstable overcharged chassis: cracked glowing core housing in the chest, venting steam and..."),
  ("cryonix", "Stocky cryogenic chassis: cryo core in the chest, frost crusted on the plating, cooling co..."),
]


def texto(resposta) -> str:
    return '\n'.join(c.text for c in resposta.content if hasattr(c, 'text'))


def id_do_prompt(t: str) -> str:
    achado = re.search(r'"prompt_id":\s*"([0-9a-f-]+)"', t) or re.search(r'[0-9a-f]{8}-[0-9a-f-]{27,}', t)
    return achado.group(1) if achado and achado.lastindex else (achado.group(0) if achado else '')


def caminhos(t: str) -> list[str]:
    try:
        dados = json.loads(t)
    except json.JSONDecodeError:
        return []
    return [f['path'] for f in dados.get('files', []) if isinstance(f, dict) and f.get('path')]


async def principal() -> int:
    so_um = sys.argv[1] if len(sys.argv) > 1 else None
    SAIDA.mkdir(parents=True, exist_ok=True)

    parametros = StdioServerParameters(command=COMFY_MCP, args=[],
                                       env={**os.environ, 'COMFY_BIN': COMFY_BIN})
    async with stdio_client(parametros) as (leitura, escrita):
        async with ClientSession(leitura, escrita) as sessao:
            await sessao.initialize()
            for identificador, conceito in CHASSIS:
                if so_um and identificador != so_um:
                    continue
                destino = SAIDA / f'silhueta_{identificador}_v001.png'
                if destino.exists():
                    print(f'  {identificador}: ja existe, pulando', flush=True)
                    continue

                prompt = (ANCORAGEM + '\n\nSilhouette sheet for a top-down action game: the same sci-fi combat'
                          ' chassis shown at the HIGH THREE-QUARTER ANGLE the player actually sees (camera about'
                          ' 50 degrees above the horizon, close to top-down), full body, neutral idle stance,'
                          ' feet on the ground. CONCEPT: ' + conceito + '.\n\n' + REGRAS)

                print(f'  {identificador}: gerando...', flush=True)
                try:
                    resposta = await sessao.call_tool('generate_image',
                                                       {'prompt': prompt, 'wait': True, 'timeout_seconds': 600})
                    conteudo = texto(resposta)
                    prompt_id = id_do_prompt(conteudo)
                    if not prompt_id:
                        print(f'    falhou: {conteudo[:140]}', flush=True)
                        continue
                    busca = await sessao.call_tool('fetch_outputs',
                                                   {'prompt_id': prompt_id, 'out_dir': str(SAIDA)})
                    arquivos = caminhos(texto(busca))
                    if arquivos and Path(arquivos[0]).exists():
                        shutil.move(arquivos[0], destino)
                        print(f'    ok -> {destino.name}', flush=True)
                    else:
                        print(f'    sem arquivo: {texto(busca)[:140]}', flush=True)
                except Exception as erro:
                    print(f'    ERRO {erro}', flush=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(asyncio.run(principal()))
