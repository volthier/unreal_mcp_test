# -*- coding: utf-8 -*-
# Gera o FRAME do painel de login conforme a spec do autor (secao 3: HARD SURFACE).
#
# Camadas, de tras para frente: placa de fundo azul-escura, miolo quase preto, blindagem com CANTOS CORTADOS
# em 45 graus e chanfros, trim ciano emissivo (com brilho borrado por baixo), accents magenta discretos,
# linhas de painel e parafusos. Sem rounded rectangle: a linguagem e hard surface sci-fi.
#
# Por que em PIL: e geometria exata (angulo de corte, espessura de chanfro, simetria dos quatro cantos). Arte
# generativa erra simetria, e um frame assimetrico desalinha o painel inteiro. O ComfyUI entra nas TEXTURAS
# (riscos, ruido, mascaras), nao na geometria.
#
# Saida 2048x1152, miolo escuro solido, para brush 9-slice (Box) com margin 0.18.

from PIL import Image, ImageDraw, ImageFilter

L, A = 2048, 1152
CIANO = (0, 229, 255)
AZUL = (0, 140, 255)
MAGENTA = (255, 46, 209)
ESCURO = (5, 11, 20)

im = Image.new('RGBA', (L, A), (0, 0, 0, 0))
d = ImageDraw.Draw(im)

M = 50
CORTE = 46


def chanfrado(x0, y0, x1, y1, c):
    # Retangulo com os quatro cantos CORTADOS em 45 graus (octogono).
    return [(x0 + c, y0), (x1 - c, y0), (x1, y0 + c), (x1, y1 - c),
            (x1 - c, y1), (x0 + c, y1), (x0, y1 - c), (x0, y0 + c)]


d.polygon(chanfrado(M, M, L - M, A - M, CORTE), fill=(9, 21, 34, 252))
d.polygon(chanfrado(M + 10, M + 10, L - M - 10, A - M - 10, CORTE - 8), fill=ESCURO + (255,))
d.polygon(chanfrado(M + 26, M + 26, L - M - 26, A - M - 26, CORTE - 20), fill=(7, 18, 29, 255))

brilho = Image.new('RGBA', (L, A), (0, 0, 0, 0))
db = ImageDraw.Draw(brilho)
db.polygon(chanfrado(M + 10, M + 10, L - M - 10, A - M - 10, CORTE - 8), outline=CIANO, width=4)
db.polygon(chanfrado(M, M, L - M, A - M, CORTE), outline=AZUL, width=3)
im.alpha_composite(brilho.filter(ImageFilter.GaussianBlur(22)))
im.alpha_composite(brilho.filter(ImageFilter.GaussianBlur(8)))
im.alpha_composite(brilho)

for (px, py, sx, sy) in ((M, M, 1, 1), (L - M, M, -1, 1), (M, A - M, 1, -1), (L - M, A - M, -1, -1)):
    d.line([(px + sx * CORTE, py), (px, py), (px, py + sy * CORTE)], fill=(140, 240, 255, 255), width=5)
    d.line([(px + sx * (CORTE + 26), py + sy * 8), (px + sx * (CORTE + 96), py + sy * 8)], fill=MAGENTA + (235,), width=4)

d.line([(M + 150, M + 64), (L - M - 150, M + 64)], fill=(20, 44, 66, 255), width=2)
d.line([(M + 150, A - M - 64), (L - M - 150, A - M - 64)], fill=(20, 44, 66, 255), width=2)
d.line([(M + 64, M + 150), (M + 64, A - M - 150)], fill=(20, 44, 66, 255), width=2)
d.line([(L - M - 64, M + 150), (L - M - 64, A - M - 150)], fill=(20, 44, 66, 255), width=2)

for (px, py) in ((M + 64, M + 64), (L - M - 64, M + 64), (M + 64, A - M - 64), (L - M - 64, A - M - 64)):
    d.ellipse([px - 7, py - 7, px + 7, py + 7], fill=(30, 58, 84, 255), outline=(90, 160, 200, 255), width=2)
    d.ellipse([px - 2, py - 2, px + 2, py + 2], fill=(12, 26, 40, 255))

cx = L // 2
d.polygon([(cx - 210, M + 10), (cx - 160, M - 18), (cx + 160, M - 18), (cx + 210, M + 10)],
          fill=(9, 21, 34, 255), outline=CIANO)
d.line([(cx - 160, M - 18), (cx + 160, M - 18)], fill=MAGENTA + (200,), width=3)

im.save('/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/ui/frame_login_v001.png')
print('frame hard-surface gerado: cantos cortados, trim ciano, magenta, linhas de painel e parafusos')
