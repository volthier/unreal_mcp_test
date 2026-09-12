# -*- coding: utf-8 -*-
# BOTAO ENTRAR conforme a spec secao 14: silhueta ANGULAR (nada de rounded rectangle), laterais recortadas,
# borda ciano emissiva, miolo escuro com gradiente azul e CHEVRONS nas laterais.
#
# Tres estados, como a secao pede: normal (glow medio), hover (glow maior e miolo mais claro) e pressed (miolo
# mais escuro). O encolhimento do pressed e no widget, nao na arte.
#
# Por que PIL: o recorte lateral tem de ser simetrico ao pixel e a borda emissiva tem de manter espessura nos
# quatro lados. E geometria, e geometria se faz com regua.

from PIL import Image, ImageDraw, ImageFilter

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/ui/'
CIANO = (0, 229, 255)
MAGENTA = (255, 46, 209)


def silhueta(x0, y0, x1, y1, corte, recuo):
    # Retangulo com cantos cortados E laterais recortadas para dentro (o degrau do alvo).
    meio = (y0 + y1) / 2
    return [(x0 + corte, y0), (x1 - corte, y0), (x1, y0 + corte),
            (x1, meio - recuo), (x1 - recuo, meio), (x1, meio + recuo),
            (x1, y1 - corte), (x1 - corte, y1), (x0 + corte, y1),
            (x0, y1 - corte), (x0, meio + recuo), (x0 + recuo, meio),
            (x0, meio - recuo), (x0, y0 + corte)]


def gerar(nome, miolo_topo, miolo_base, borda, forca_glow, altura=200, largura=1200):
    im = Image.new('RGBA', (largura, altura), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    M = 16
    forma = silhueta(M, M, largura - M, altura - M, 26, 16)

    faixa = Image.new('RGB', (1, altura))
    for y in range(altura):
        t = y / max(1, altura - 1)
        faixa.putpixel((0, y), tuple(int(miolo_topo[i] * (1 - t) + miolo_base[i] * t) for i in range(3)))
    mascara = Image.new('L', (largura, altura), 0)
    ImageDraw.Draw(mascara).polygon(forma, fill=255)
    im.paste(faixa.resize((largura, altura)).convert('RGBA'), (0, 0), mascara)

    brilho = Image.new('RGBA', (largura, altura), (0, 0, 0, 0))
    ImageDraw.Draw(brilho).polygon(forma, outline=borda, width=5)
    im.alpha_composite(brilho.filter(ImageFilter.GaussianBlur(int(18 * forca_glow))))
    im.alpha_composite(brilho.filter(ImageFilter.GaussianBlur(4)))
    im.alpha_composite(brilho)

    # a linha de luz interna do topo
    d.line([(M + 60, M + 11), (largura - M - 60, M + 11)], fill=(120, 220, 255, 80), width=3)

    # CHEVRONS nas laterais (secao 14)
    for (px, direcao) in ((M + 50, 1), (largura - M - 50, -1)):
        cy = altura // 2
        d.line([(px - direcao * 10, cy - 15), (px + direcao * 7, cy), (px - direcao * 10, cy + 15)],
               fill=borda, width=6, joint='curve')

    im.save(RAIZ + nome + '.png')


gerar('btn_entrar_normal', (10, 22, 40), (14, 40, 70), CIANO, 1.0)
gerar('btn_entrar_hover', (16, 44, 78), (22, 70, 118), (140, 245, 255), 1.5)
gerar('btn_entrar_pressed', (5, 14, 26), (8, 26, 46), (0, 180, 210), 0.6)
gerar('btn_secundario_normal', (8, 16, 30), (10, 22, 40), MAGENTA, 0.7)
print('botoes gerados: entrar (normal, hover, pressed) e secundario')
