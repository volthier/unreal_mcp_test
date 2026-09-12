# -*- coding: utf-8 -*-
# Gera o LOGOTIPO do alvo: AETHER FORGE + X, na tipografia da spec (Orbitron) e com o X como elemento
# mais energetico da marca (secao 8: X grande, angular, energia ciano + magenta, alto contraste).
#
# O X e da NOSSA marca - nao tem vinculo com marca de terceiros -, entao ele entra sem ressalva. O que a
# regra do projeto proibe e asset de terceiros, e nao ha nenhum aqui.
#
# As fontes sao variaveis (Google Fonts, OFL), e o peso vem do eixo wght.

from PIL import Image, ImageDraw, ImageFilter, ImageFont

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/ui/'
ORBITRON = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/fontes/Orbitron.ttf'
CIANO = (0, 229, 255)
AZUL = (0, 140, 255)
MAGENTA = (255, 46, 209)
PRATA = (234, 242, 255)


def fonte(tamanho, peso):
    f = ImageFont.truetype(ORBITRON, tamanho)
    try:
        f.set_variation_by_axes([peso])
    except Exception:
        pass
    return f


L, A = 2000, 560
logo = Image.new('RGBA', (L, A), (0, 0, 0, 0))
d = ImageDraw.Draw(logo)

f_titulo = fonte(190, 900)
f_x = fonte(268, 900)
f_sub = fonte(58, 600)

titulo = 'AETHER FORGE'
caixa = f_titulo.getbbox(titulo)
largura_titulo = caixa[2] - caixa[0]
caixa_x = f_x.getbbox('X')
largura_x = caixa_x[2] - caixa_x[0]

# o conjunto (titulo + X) fica centrado
espaco = 34
largura_total = largura_titulo + espaco + largura_x
x0 = (L - largura_total) // 2 - caixa[0]
y0 = 26

# 1. o titulo em PRATA, como a spec pede (metal claro / prata / azul)
d.text((x0, y0), titulo, font=f_titulo, fill=PRATA + (255,))

# 2. o X: degradê ciano -> magenta com halo. E o elemento mais energetico da logo.
x_x = x0 + largura_titulo + espaco
mascara = Image.new('L', (L, A), 0)
ImageDraw.Draw(mascara).text((x_x, y0 - 22), 'X', font=f_x, fill=255)
faixa = Image.new('RGB', (L, 1))
for px in range(L):
    t = px / max(1, L - 1)
    faixa.putpixel((px, 0), (int(CIANO[0] * (1 - t) + MAGENTA[0] * t),
                            int(CIANO[1] * (1 - t) + MAGENTA[1] * t),
                            int(CIANO[2] * (1 - t) + MAGENTA[2] * t)))
pintura = faixa.resize((L, A)).convert('RGBA')
halo = Image.new('RGBA', (L, A), (0, 0, 0, 0))
halo.paste(pintura, (0, 0), mascara.filter(ImageFilter.GaussianBlur(26)))
logo.alpha_composite(halo)
logo.paste(pintura, (0, 0), mascara)

# 3. PROTOCOL ZERO espacado, em ciano
sub = 'P R O T O C O L   Z E R O'
largura_sub = d.textlength(sub, font=f_sub)
d.text(((L - largura_sub) / 2 + 40, 300), sub, font=f_sub, fill=CIANO + (255,))

logo.save(RAIZ + 'logo_aether.png')
print('logo AETHER FORGE X gerado: titulo em prata, X em ciano-magenta, PROTOCOL ZERO espacado')
