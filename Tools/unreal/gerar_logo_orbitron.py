# -*- coding: utf-8 -*-
'''Gera o LOGOTIPO na fonte especificada pelo guia: Orbitron (titulos) e Exo 2 (UI).

Antes eu usei a DIN Alternate Bold do macOS, que era a mais proxima disponivel. Agora a Orbitron de verdade
esta em Art/fontes (Google Fonts, licenca OFL - pode ser usada e redistribuida no projeto).

As duas sao fontes VARIAVEIS, entao o peso vem do eixo wght: 900 (Black) no titulo e 600 no subtitulo. O
Unreal importa a instancia padrao da variavel, e por isso o TITULO fica como imagem (onde eu controlo o peso
exato) e a UI usa a fonte importada.
'''

from PIL import Image, ImageDraw, ImageFilter, ImageFont

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/ui/'
ORBITRON = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/fontes/Orbitron.ttf'
CIANO = (0, 229, 255)
VIOLETA = (168, 85, 247)
ROSA = (255, 46, 136)
BRANCO = (255, 255, 255)


def fonte(caminho, tamanho, peso):
    f = ImageFont.truetype(caminho, tamanho)
    try:
        f.set_variation_by_axes([peso])
    except Exception:
        pass
    return f


def degrade(largura, altura, cores):
    faixa = Image.new('RGB', (largura, 1))
    for x in range(largura):
        t = x / max(1, largura - 1)
        trecho = t * (len(cores) - 1)
        i = min(int(trecho), len(cores) - 2)
        f = trecho - i
        faixa.putpixel((x, 0), tuple(int(cores[i][k] * (1 - f) + cores[i + 1][k] * f) for k in range(3)))
    return faixa.resize((largura, altura))


# AETHER FORGE em Orbitron 900, com o X em destaque no degrade (como no alvo do autor)
L, A = 1800, 520
logo = Image.new('RGBA', (L, A), (0, 0, 0, 0))
f_titulo = fonte(ORBITRON, 190, 900)
f_sub = fonte(ORBITRON, 62, 600)

texto = 'AETHER FORGE'
caixa = f_titulo.getbbox(texto)
x0 = (L - (caixa[2] - caixa[0])) // 2 - caixa[0]
y0 = 20

mascara = Image.new('L', (L, A), 0)
ImageDraw.Draw(mascara).text((x0, y0), texto, font=f_titulo, fill=255)
pintura = degrade(L, A, [CIANO, (140, 210, 255), VIOLETA, ROSA]).convert('RGBA')
halo = Image.new('RGBA', (L, A), (0, 0, 0, 0))
halo.paste(degrade(L, A, [CIANO, VIOLETA, ROSA]).convert('RGBA'), (0, 0), mascara.filter(ImageFilter.GaussianBlur(22)))
logo.alpha_composite(halo)
logo.paste(pintura, (0, 0), mascara)
d = ImageDraw.Draw(logo)
sub = 'P R O T O C O L   Z E R O'
largura_sub = d.textlength(sub, font=f_sub)
d.text(((L - largura_sub) / 2, 268), sub, font=f_sub, fill=CIANO + (255,))
logo.save(RAIZ + 'logo_aether.png')
print('logotipo em Orbitron Black, com o subtitulo em Orbitron 600')
