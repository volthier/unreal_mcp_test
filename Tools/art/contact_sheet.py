#!/usr/bin/env python3
"""Monta uma folha de contato dos assets gerados - e a versao em SILHUETA PURA.

Por que existe: com camera alta o que informa e a silhueta (Docs/Modelo_Interacao.md). Entao a
pergunta certa nao e 'ficou bonito?', e 'da para saber quem e, so pelo contorno?'. Este script
responde isso: gera a folha colorida e a folha com cada asset reduzido a preto no branco.

Uso: python3 Tools/art/contact_sheet.py <pasta> <prefixo_saida> [colunas]
     python3 Tools/art/contact_sheet.py Art/generated/chassis contato_chassis 4
"""

from __future__ import annotations

import sys
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont, ImageOps

FUNDO = (26, 26, 46)        # silhueta da paleta #1a1a2e
TEXTO = (216, 200, 144)     # rebites #d8c890
LATAO = (201, 162, 39)      # latao polido #c9a227
LADO = 420
MARGEM = 14
FAIXA = 34


def fonte(tamanho: int):
    for caminho in ('/System/Library/Fonts/Supplemental/Arial Bold.ttf',
                    '/System/Library/Fonts/Helvetica.ttc'):
        try:
            return ImageFont.truetype(caminho, tamanho)
        except Exception:
            continue
    return ImageFont.load_default()


def montar(arquivos: list[Path], colunas: int, destino: Path, so_silhueta: bool) -> None:
    linhas = (len(arquivos) + colunas - 1) // colunas
    largura = MARGEM + colunas * (LADO + MARGEM)
    altura = MARGEM + linhas * (LADO + FAIXA + MARGEM)
    folha = Image.new('RGB', (largura, altura), FUNDO)
    desenho = ImageDraw.Draw(folha)
    fonte_nome = fonte(22)

    for indice, caminho in enumerate(arquivos):
        coluna = indice % colunas
        linha = indice // colunas
        x = MARGEM + coluna * (LADO + MARGEM)
        y = MARGEM + linha * (LADO + FAIXA + MARGEM)

        with Image.open(caminho) as imagem:
            recorte = ImageOps.fit(imagem.convert('RGB'), (LADO, LADO), Image.LANCZOS)
        if so_silhueta:
            # reduz a contorno: o que sobrar preto e o que o jogador vai reconhecer de cima
            cinza = ImageOps.grayscale(recorte)
            recorte = cinza.point(lambda v: 255 if v > 96 else 0).convert('RGB')

        folha.paste(recorte, (x, y))
        desenho.rectangle([x, y, x + LADO, y + LADO], outline=LATAO, width=2)

        nome = caminho.stem.replace('silhueta_', '').replace('_v001', '').upper()
        desenho.text((x + 4, y + LADO + 6), nome, font=fonte_nome, fill=TEXTO)

    destino.parent.mkdir(parents=True, exist_ok=True)
    folha.save(destino)
    print(f'  {destino}  ({largura}x{altura}, {len(arquivos)} itens)')


def principal() -> int:
    pasta = Path(sys.argv[1]) if len(sys.argv) > 1 else Path('Art/generated/chassis')
    prefixo = sys.argv[2] if len(sys.argv) > 2 else 'contato'
    colunas = int(sys.argv[3]) if len(sys.argv) > 3 else 4

    arquivos = sorted(p for p in pasta.glob('*.png') if not p.name.startswith('_'))
    if not arquivos:
        print('nenhuma imagem encontrada em', pasta)
        return 1

    montar(arquivos, colunas, pasta / f'_{prefixo}_cor.png', so_silhueta=False)
    montar(arquivos, colunas, pasta / f'_{prefixo}_silhueta.png', so_silhueta=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(principal())
