#!/usr/bin/env python3
'''Gera o NUCLEO de cada chassi: o cristal com a aura de gelo, na cor daquele chassi.

Por que existe (e por que e assim):
- o chassi E o cristal; o corpo e da CLASSE (Docs/Prompts_Arte_Ploidrek.md 9b.1);
- a COR do nucleo e identidade fixa, ligada a habilidade (secao 9b, aprovada pelo autor) - por isso a cor
  entra no prompt como RGB exato, e nao como 'azul' ou 'vermelho';
- a aura e a nuvem de gelo que gira em volta, que no jogo e a casca aditiva com a textura de geada
  (M_AuraGeada) e, por cima, o sistema Niagara quando estiver salvo em disco;
- a fonte da cor e o Data/DT_Chassis.csv (coluna CoreColor): dado, nao gosto.

Uso: python3 Tools/comfy/gerar_nucleos.py [id_do_chassi]
'''

from __future__ import annotations

import csv
import subprocess
import sys
from pathlib import Path

RAIZ = Path('/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST')
CSV = RAIZ / 'Data/DT_Chassis.csv'
SAIDA = RAIZ / 'Art/generated/nucleos'

DESCRICAO = {
    ("Charger", "255,106,0", "molten impact orange"),
    ("Vitaspark", "255,217,26", "electric spark yellow"),
    ("Aetheric", "64,255,192", "aether green"),
    ("Techno", "38,115,255", "data blue"),
    ("Droneframe", "154,217,76", "sensor green"),
    ("Forgekin", "179,92,30", "forge bronze"),
    ("Ghostnet", "153,76,230", "ghost violet"),
    ("Vox", "255,76,178", "voice magenta"),
    ("Overcore", "255,240,196", "incandescent white"),
    ("Cryonix", "94,200,255", "cryogenic ice blue"),
}


def cor_do_csv(nome: str) -> str:
    with CSV.open(encoding='utf-8-sig') as arquivo:
        for linha in csv.DictReader(arquivo):
            if linha['Name'] != nome:
                continue
            texto = (linha.get('CoreColor') or '').strip().strip('()')
            valores = {}
            for parte in texto.split(','):
                if '=' in parte:
                    chave, valor = parte.split('=', 1)
                    valores[chave.strip()] = float(valor)
            if {'R', 'G', 'B'} <= set(valores):
                return '%d,%d,%d' % (round(valores['R'] * 255), round(valores['G'] * 255), round(valores['B'] * 255))
    return '255,255,255'


def prompt(nome: str) -> str:
    rgb = cor_do_csv(nome)
    desc = DESCRICAO.get(nome, 'a distinctive colour')[0] if False else [d for c, r, d in [(n, None, d) for n, r, d in DESCRICAO if n == nome]][0]
    return chr(10).join([
        'A single game asset icon: the NUCLEUS of a combat chassis - a small faceted CRYSTAL CORE, self-lit and',
        'floating, wrapped in a cold mist aura that behaves like wind (icy vapour, glowing filaments circling the',
        'crystal, like the ice-bloom that grows around frozen surfaces).',
        'The crystal and its aura are ' + desc + ' (RGB ' + rgb + '), glowing strongly from inside, with a subtle',
        'brass bezel ring where the crystal seats. Dark neutral background, three-quarter view, cinematic studio',
        'light, high detail, sharp facets. Centred, the crystal filling most of the frame.',
        'IMPORTANT: no text, no letters, no words, no numbers, no logos, no watermark, no characters.',
    ])


def principal() -> int:
    so_um = sys.argv[1] if len(sys.argv) > 1 else None
    SAIDA.mkdir(parents=True, exist_ok=True)
    for indice, (nome, _rgb, _desc) in enumerate(DESCRICAO):
        if so_um and nome != so_um:
            continue
        destino = SAIDA / ('nucleo_' + nome.lower() + '_v001.png')
        if destino.exists():
            print('  ' + nome + ': ja existe, pulando')
            continue
        comando = ['python3', str(RAIZ / 'Tools/comfy/txt2img.py'),
                   '--prompt', prompt(nome), '--out', str(destino),
                   '--largura', '768', '--altura', '768', '--semente', str(600 + indice)]
        print('  ' + nome + ' (cor ' + cor_do_csv(nome) + '): gerando...', flush=True)
        subprocess.run(comando, capture_output=True, text=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(principal())
