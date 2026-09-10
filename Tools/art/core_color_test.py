#!/usr/bin/env python3
"""Testa a ideia do autor: o nucleo do chassi carrega a cor de identidade dele.

Desenha o nucleo de cada chassi (AccentColor do DataTable) em TRES tamanhos, incluindo o tamanho
em que ele aparece com a camera alta e distante (24 px). A pergunta que a imagem responde nao e
'a cor e bonita?', e 'da para dizer quem e, de longe?'. Cor que colide em distancia falha aqui,
e nao no jogo.

Uso: python3 Tools/art/core_color_test.py
"""

from __future__ import annotations

import csv
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter, ImageFont

FUNDO = (26, 26, 46)
TEXTO = (216, 200, 144)
LATAO = (201, 162, 39)
TAMANHOS = (128, 56, 24)
COLUNAS = 5


def cor_de(texto: str):
    if not texto:
        return None
    valores = {}
    for parte in texto.strip().strip('()').split(','):
        if '=' in parte:
            chave, valor = parte.split('=', 1)
            valores[chave.strip()] = float(valor)
    if {'R', 'G', 'B'} <= set(valores):
        return (round(valores['R'] * 255), round(valores['G'] * 255), round(valores['B'] * 255))
    return None


def nucleo(tamanho: int, cor):
    """Um nucleo: brilho difuso + disco cheio + aro claro, como o renderizou na arte."""
    lado = tamanho * 3
    camada = Image.new('RGBA', (lado, lado), (0, 0, 0, 0))
    desenho = ImageDraw.Draw(camada)
    centro = lado // 2
    for raio, alfa in ((tamanho, 90), (int(tamanho * 0.75), 150), (int(tamanho * 0.55), 255)):
        desenho.ellipse([centro - raio, centro - raio, centro + raio, centro + raio],
                        fill=(cor[0], cor[1], cor[2], alfa))
    desenho.ellipse([centro - tamanho * 0.28, centro - tamanho * 0.28,
                     centro + tamanho * 0.28, centro + tamanho * 0.28],
                    fill=(min(255, cor[0] + 70), min(255, cor[1] + 70), min(255, cor[2] + 70), 255))
    return camada.filter(ImageFilter.GaussianBlur(tamanho * 0.06))


def principal() -> int:
    caminho = Path('Data/DT_Chassis.csv')
    chassis = []
    with caminho.open(encoding='utf-8-sig') as arquivo:
        for linha in csv.DictReader(arquivo):
            cor = cor_de(linha.get('AccentColor'))
            if cor:
                chassis.append((linha['Name'], cor, linha['Body']))

    largura, altura = 1360, 640
    folha = Image.new('RGB', (largura, altura), FUNDO)
    desenho = ImageDraw.Draw(folha)
    try:
        fonte = ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial Bold.ttf', 20)
        fonte_min = ImageFont.truetype('/System/Library/Fonts/Supplemental/Arial.ttf', 15)
    except Exception:
        fonte = fonte_min = ImageFont.load_default()

    coluna = 0
    linha_atual = 0
    for nome, cor, corpo in chassis:
        x = 40 + coluna * 260
        y = 40 + linha_atual * 150
        for i, tamanho in enumerate(TAMANHOS):
            px = x + i * 62
            py = y + 30
            brilho = nucleo(tamanho, cor)
            folha.paste(brilho, (px - tamanho, py - tamanho), brilho)
        desenho.text((x, y + 105), nome, font=fonte, fill=TEXTO)
        desenho.text((x, y + 128), f"#{cor[0]:02x}{cor[1]:02x}{cor[2]:02x}  {corpo}", font=fonte_min, fill=LATAO)
        coluna += 1
        if coluna == COLUNAS:
            coluna = 0
            linha_atual += 1

    destino = Path('Art/generated/chassis/_teste_nucleo.png')
    folha.save(destino)
    print(f'  {destino}  ({largura}x{altura}) - 128 px, 56 px e 24 px (tamanho de jogo)')
    return 0


if __name__ == '__main__':
    raise SystemExit(principal())
