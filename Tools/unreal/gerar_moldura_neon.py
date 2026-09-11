# -*- coding: utf-8 -*-
'''Desenha a MOLDURA do painel de login conforme o guia (Art/Tela_login): vidro escuro com borda neon.

Por que em PIL e nao em geracao por IA: isto e interface, nao arte de cena. A geometria tem de ser exata -
canto arredondado com o mesmo raio nos quatro lados, clipping no topo, brilho simetrico - e a imagem tem de
servir de brush com margem 9 (Box). Arte generativa aqui traria borda torta e o painel ficaria torto.

Saida: 2048x1152 com o miolo PRETO PURO e a borda desenhada. O miolo vira a cor de fundo do painel pelo
brush (o miolo do brush e o que preenche a area), entao ele precisa ser escuro - foi o interior BRANCO da
moldura antiga que deixou a tela ilegivel.
'''

from PIL import Image, ImageDraw, ImageFilter

L, A = 2048, 1152
MARGEM = 74          # espessura da moldura (o brush usa a mesma margem em fracao)
RAIO = 46
CIANO = (0, 229, 255)
VIOLETA = (168, 85, 247)
ROSA = (255, 46, 136)
VIDRO = (10, 16, 32, 255)

im = Image.new('RGBA', (L, A), (0, 0, 0, 0))
d = ImageDraw.Draw(im)

# 1. o vidro: retangulo escuro de canto arredondado, com uma linha interna mais clara (vidro com profundidade)
d.rounded_rectangle([MARGEM, MARGEM, L - MARGEM, A - MARGEM], radius=RAIO, fill=VIDRO,
                    outline=(28, 40, 66, 255), width=3)
d.rounded_rectangle([MARGEM + 12, MARGEM + 12, L - MARGEM - 12, A - MARGEM - 12], radius=RAIO - 10,
                    outline=(20, 30, 52, 255), width=2)

# 2. as bordas neon: ciano por fora, violeta por dentro - o brilho vem de uma copia borrada por baixo
brilho = Image.new('RGBA', (L, A), (0, 0, 0, 0))
db = ImageDraw.Draw(brilho)
db.rounded_rectangle([MARGEM, MARGEM, L - MARGEM, A - MARGEM], radius=RAIO, outline=CIANO, width=5)
db.rounded_rectangle([MARGEM + 14, MARGEM + 14, L - MARGEM - 14, A - MARGEM - 14], radius=RAIO - 12,
                     outline=VIOLETA, width=3)
im.alpha_composite(brilho.filter(ImageFilter.GaussianBlur(16)))
im.alpha_composite(brilho.filter(ImageFilter.GaussianBlur(6)))
im.alpha_composite(brilho)

# 3. o clipping do topo, como no guia: um recorte angular no meio da aresta superior
cx = L // 2
d.polygon([(cx - 190, MARGEM), (cx - 150, MARGEM - 30), (cx + 150, MARGEM - 30), (cx + 190, MARGEM)],
          fill=VIDRO, outline=CIANO)
d.line([(cx - 150, MARGEM - 30), (cx + 150, MARGEM - 30)], fill=VIOLETA, width=4)

# 4. acentos nos quatro cantos (as diagonais curtas do guia)
canto = 96
for (px, py, sx, sy) in ((MARGEM, MARGEM, 1, 1), (L - MARGEM, MARGEM, -1, 1),
                         (MARGEM, A - MARGEM, 1, -1), (L - MARGEM, A - MARGEM, -1, -1)):
    d.line([(px, py + sy * canto), (px, py), (px + sx * canto, py)], fill=ROSA, width=5)

im.save('/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/ui/kit_moldura_neon_v001.png')
print('moldura neon: miolo escuro, borda ciano e violeta, clipping no topo, cantos rosa')
