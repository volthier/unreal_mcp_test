#!/usr/bin/env python3
"""Desenha uma SIMULACAO do veu do menu, com a mesma matematica de RunnerVeilWidget.cpp.

Serve para conferir o efeito sem rodar o jogo (e para ajustar raio/alpha com o autor).
Nao e o jogo: e uma aproximacao fiel da composicao (blobs de gradiente radial + blend alfa).

Uso:  python3 Tools/preview/veil_preview.py [saida.png]
"""

import math
import random
import struct
import sys
import zlib
from pathlib import Path

LARGURA, ALTURA = 1152, 648
RAIO_TEX = 128

# Paleta canonica (Docs/SteampunkPalette.md)
LATAO = (0xc9, 0xa2, 0x27)
LATAO_ESCURO = (0xa8, 0x84, 0x2c)
FERRO = (0x3a, 0x3a, 0x42)
ACO = (0x2a, 0x2a, 0x30)
SILHUETA = (0x1a, 0x1a, 0x2e)
AMBAR = (0xff, 0xb0, 0x40)
VAPOR = (0xc8, 0xc0, 0xa8)
FORJA = (0xc8, 0x6a, 0x30)
REBITES = (0xd8, 0xc8, 0x90)
CEU_TOPO = (0x1a, 0x2a, 0x5e)
CEU_MEIO = (0x3a, 0x4a, 0x80)


def textura_neblina():
    """Gradiente radial identico ao do C++: (1 - d) ** 2.2 em alfa, branco no RGB."""
    centro = RAIO_TEX * 0.5
    tex = []
    for y in range(RAIO_TEX):
        linha = []
        for x in range(RAIO_TEX):
            dx = (x + 0.5 - centro) / centro
            dy = (y + 0.5 - centro) / centro
            d = math.sqrt(dx * dx + dy * dy)
            queda = max(0.0, min(1.0, 1.0 - d))
            linha.append(queda ** 2.2)
        tex.append(linha)
    return tex


TEX = textura_neblina()


def amostra(u, v):
    """Bilinear na textura (Slate usa bilinear)."""
    if u < 0.0 or u > 1.0 or v < 0.0 or v > 1.0:
        return 0.0
    x = u * (RAIO_TEX - 1)
    y = v * (RAIO_TEX - 1)
    x0, y0 = int(x), int(y)
    x1, y1 = min(x0 + 1, RAIO_TEX - 1), min(y0 + 1, RAIO_TEX - 1)
    fx, fy = x - x0, y - y0
    a = TEX[y0][x0] * (1 - fx) + TEX[y0][x1] * fx
    b = TEX[y1][x0] * (1 - fx) + TEX[y1][x1] * fx
    return a * (1 - fy) + b * fy


def desenhar_neblina(buf, cx, cy, raio, cor, alfa):
    if alfa <= 0.0 or raio <= 1.0:
        return
    x0 = max(0, int(cx - raio))
    x1 = min(LARGURA - 1, int(cx + raio))
    y0 = max(0, int(cy - raio))
    y1 = min(ALTURA - 1, int(cy + raio))
    inv = 1.0 / (raio * 2.0)
    for y in range(y0, y1 + 1):
        linha = buf[y]
        v = (y - (cy - raio)) * inv
        for x in range(x0, x1 + 1):
            u = (x - (cx - raio)) * inv
            a = amostra(u, v) * alfa
            if a <= 0.002:
                continue
            p = linha[x]
            linha[x] = (
                p[0] + (cor[0] - p[0]) * a,
                p[1] + (cor[1] - p[1]) * a,
                p[2] + (cor[2] - p[2]) * a,
            )


def retangulo(buf, x0, y0, x1, y1, cor, raio=0, contorno=None, esp=1.5):
    for y in range(max(0, int(y0)), min(ALTURA - 1, int(y1)) + 1):
        for x in range(max(0, int(x0)), min(LARGURA - 1, int(x1)) + 1):
            dx = max(x0 + raio - x, 0, x - (x1 - raio))
            dy = max(y0 + raio - y, 0, y - (y1 - raio))
            d = math.sqrt(dx * dx + dy * dy)
            dentro = d <= raio
            if contorno is not None and raio > 0:
                borda = abs(d - raio) <= esp and x0 <= x <= x1 and y0 <= y <= y1
                if borda:
                    buf[y][x] = contorno
                    continue
            if dentro:
                buf[y][x] = cor


def escrever_png(caminho, buf):
    linhas = bytearray()
    for linha in buf:
        linhas.append(0)
        for (r, g, b) in linha:
            linhas += bytes((int(max(0, min(255, r))), int(max(0, min(255, g))), int(max(0, min(255, b)))))

    def bloco(tipo, dados):
        return (struct.pack(">I", len(dados)) + tipo + dados
                + struct.pack(">I", zlib.crc32(tipo + dados) & 0xFFFFFFFF))

    png = b"\x89PNG\r\n\x1a\n"
    png += bloco(b"IHDR", struct.pack(">IIBBBBB", LARGURA, ALTURA, 8, 2, 0, 0, 0))
    png += bloco(b"IDAT", zlib.compress(bytes(linhas), 6))
    png += bloco(b"IEND", b"")
    Path(caminho).write_bytes(png)


def main():
    saida = sys.argv[1] if len(sys.argv) > 1 else "Saved/Preview_Veu.png"

    # Fundo: ceu dusk + horizonte de forja (o mapa do jogo ao fundo do menu).
    buf = []
    for y in range(ALTURA):
        t = y / (ALTURA - 1)
        if t < 0.62:
            k = t / 0.62
            cor = tuple(CEU_TOPO[i] + (CEU_MEIO[i] - CEU_TOPO[i]) * k for i in range(3))
        else:
            k = (t - 0.62) / 0.38
            cor = tuple(CEU_MEIO[i] + (FORJA[i] - CEU_MEIO[i]) * (k * 0.55) for i in range(3))
        buf.append([cor for _ in range(LARGURA)])

    # Painel do menu (680x620 centrado, como no C++).
    px0 = (LARGURA - 680) // 2
    py0 = (ALTURA - 620) // 2
    px1, py1 = px0 + 680, py0 + 620
    retangulo(buf, px0, py0, px1, py1, SILHUETA, raio=12, contorno=LATAO_ESCURO, esp=1.5)

    # Filete de latao do cabecalho e blocos de conteudo.
    retangulo(buf, px0 + 20, py0 + 74, px1 - 20, py0 + 76, LATAO_ESCURO, raio=1)

    botoes = []
    y = py0 + 96
    for _ in range(4):  # campos/opcoes
        botoes.append((px0 + 24, y, px1 - 24, y + 34))
        y += 44
    y += 10
    botoes.append((px0 + 24, y, px1 - 24, y + 42))       # principal (ambar)
    botoes.append((px0 + 24, y + 50, px1 - 24, y + 88))  # secundario
    for indice, (bx0, by0, bx1, by1) in enumerate(botoes):
        if indice == 4:
            retangulo(buf, bx0, by0, bx1, by1, AMBAR, raio=6)
        elif indice == 5:
            retangulo(buf, bx0, by0, bx1, by1, FERRO, raio=6, contorno=LATAO_ESCURO, esp=1)
        else:
            retangulo(buf, bx0, by0, bx1, by1, ACO, raio=6, contorno=(0x60, 0x58, 0x3a), esp=1)

    # ---------------- O VEU (mesma matematica do C++) ----------------
    tempo = 12.7
    mouse = (int(LARGURA * 0.62), int(ALTURA * 0.46))

    # 1. NUVENS que voam e dão a volta pelas bordas (semente fixa, como no C++).
    sorteio = random.Random(20260913)
    margem = 520.0
    faixa_x, faixa_y = LARGURA + margem * 2, ALTURA + margem * 2
    for _ in range(14):
        base = (sorteio.uniform(-0.2, 1.2), sorteio.uniform(-0.15, 1.15))
        inclinacao = sorteio.uniform(-0.30, 0.30)
        sentido = 1.0 if sorteio.random() < 0.5 else -1.0
        direcao = (math.cos(inclinacao) * sentido, math.sin(inclinacao))
        velocidade = sorteio.uniform(0.012, 0.048)
        escala = sorteio.uniform(170, 430)
        fase = sorteio.uniform(0, 120)
        escurece = sorteio.random() < 0.4

        x = (base[0] * faixa_x + direcao[0] * velocidade * faixa_x * tempo) % faixa_x
        y = (base[1] * faixa_y + direcao[1] * velocidade * faixa_y * tempo) % faixa_y
        cx, cy = x - margem, y - margem
        raio = escala * (1.0 + 0.14 * math.sin(tempo * 0.22 + fase))
        cor = SILHUETA if escurece else VAPOR
        alfa = 0.17 if escurece else 0.10

        desenhar_neblina(buf, cx, cy, raio, cor, alfa)
        desenhar_neblina(buf, cx + raio * 0.58, cy - raio * 0.20, raio * 0.70, cor, alfa)
        desenhar_neblina(buf, cx - raio * 0.52, cy + raio * 0.24, raio * 0.62, cor, alfa)

    # 2. fios rondando os botoes (raio maior que o botao: a neblina passa por cima)
    for indice, (bx0, by0, bx1, by1) in enumerate(botoes):
        fase = indice * 1.7
        cax, cay = (bx0 + bx1) * 0.5, (by0 + by1) * 0.5
        raiox = (bx1 - bx0) * 0.5 + 55
        raioy = (by1 - by0) * 0.5 + 40
        vx = cax + math.cos(tempo * 0.9 + fase) * raiox
        vy = cay + math.sin(tempo * 1.25 + fase) * raioy
        desenhar_neblina(buf, vx, vy, 95 + 35 * math.sin(tempo * 0.8 + fase), VAPOR, 0.07)

    # 3. A dispersao abre no cursor: grande e macia (não um foco de lanterna).
    respiro = 1.0 + 0.06 * math.sin(tempo * 1.3)
    desenhar_neblina(buf, mouse[0], mouse[1], 300 * 1.90 * respiro, VAPOR, 0.13)
    desenhar_neblina(buf, mouse[0], mouse[1], 300 * 1.15 * respiro, VAPOR, 0.24)
    desenhar_neblina(buf, mouse[0], mouse[1], 300 * 0.50 * respiro, FORJA, 0.20)

    escrever_png(saida, buf)
    print("previa escrita em " + saida)


if __name__ == "__main__":
    main()
