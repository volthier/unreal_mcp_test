# -*- coding: utf-8 -*-
'''Gera o kit da tela de login conforme o alvo do autor (Art/Tela_login):
   ui_logo_aether      - o logotipo AETHER FORGE com o X em degrade, e PROTOCOL ZERO espacado;
   ui_icone_pessoa     - o icone do campo de usuario;
   ui_icone_cadeado    - o icone do campo de senha;
   ui_moldura_latao    - (substitui) o painel de HUD: canto arredondado, borda ciano fina e COLCHETES de canto.

Por que em PIL e nao por IA: interface precisa de geometria exata e de texto legivel. Gerador de imagem
erra letra e entorta borda - e uma moldura torta desalinha o painel inteiro.

Fonte: DIN Alternate Bold, do proprio macOS. Nao e a Orbitron do guia (que nao existe nesta maquina), mas e
a geometria tecnica mais proxima disponivel. Trocar por Orbitron depois e substituir o PNG.
'''

from PIL import Image, ImageDraw, ImageFilter, ImageFont

RAIZ = '/Users/volthier/Documents/Unreal Projects/AI_MEGA_MAN_TEST/Art/generated/ui/'
FONTE = '/System/Library/Fonts/Supplemental/DIN Alternate Bold.ttf'
CIANO = (0, 229, 255)
VIOLETA = (168, 85, 247)
ROSA = (255, 46, 136)
BRANCO = (255, 255, 255)


def degradê(largura, altura, cores):
    '''Degrade horizontal entre as cores dadas (lista de RGB).'''
    faixa = Image.new('RGB', (largura, 1))
    for x in range(largura):
        t = x / max(1, largura - 1)
        trecho = t * (len(cores) - 1)
        i = min(int(trecho), len(cores) - 2)
        f = trecho - i
        c = tuple(int(cores[i][k] * (1 - f) + cores[i + 1][k] * f) for k in range(3))
        faixa.putpixel((x, 0), c)
    return faixa.resize((largura, altura))


# ---------------- 1. O LOGOTIPO ----------------
L, A = 1600, 560
logo = Image.new('RGBA', (L, A), (0, 0, 0, 0))

fonte_titulo = ImageFont.truetype(FONTE, 210)
fonte_sub = ImageFont.truetype(FONTE, 74)

texto = 'AETHER FORGE'
caixa = fonte_titulo.getbbox(texto)
largura_texto = caixa[2] - caixa[0]
x0 = (L - largura_texto) // 2 - caixa[0]
y0 = 30

# o titulo inteiro em degrade ciano -> violeta -> rosa, como o X do alvo
mascara = Image.new('L', (L, A), 0)
ImageDraw.Draw(mascara).text((x0, y0), texto, font=fonte_titulo, fill=255)
pintura = degradê(L, A, [CIANO, (120, 200, 255), VIOLETA, ROSA]).convert('RGBA')
brilho = mascara.filter(ImageFilter.GaussianBlur(18))
halo = Image.new('RGBA', (L, A), (0, 0, 0, 0))
halo.paste(degradê(L, A, [CIANO, VIOLETA, ROSA]).convert('RGBA'), (0, 0), brilho)
logo.alpha_composite(halo)
logo.paste(pintura, (0, 0), mascara)

# contorno branco fino por dentro: da o acabamento metalico do alvo
contorno = mascara.filter(ImageFilter.MaxFilter(3))
borda = Image.new('RGBA', (L, A), (0, 0, 0, 0))
borda.paste(Image.new('RGBA', (L, A), BRANCO + (150,)), (0, 0), contorno)
logo.alpha_composite(Image.composite(borda, Image.new('RGBA', (L, A), (0, 0, 0, 0)), mascara))

# PROTOCOL ZERO espacado, em ciano
d = ImageDraw.Draw(logo)
sub = 'P R O T O C O L   Z E R O'
largura_sub = d.textlength(sub, font=fonte_sub)
d.text(((L - largura_sub) / 2, 290), sub, font=fonte_sub, fill=CIANO + (255,))
logo.save(RAIZ + 'logo_aether.png')


# ---------------- 2. OS ICONES DOS CAMPOS ----------------
def salvar_icone(nome, desenhar):
    im = Image.new('RGBA', (128, 128), (0, 0, 0, 0))
    dd = ImageDraw.Draw(im)
    desenhar(dd)
    im.save(RAIZ + nome + '.png')


# pessoa: cabeca redonda + ombros em arco
def pessoa(dd):
    dd.ellipse([46, 20, 82, 56], outline=CIANO, width=7)
    dd.arc([26, 66, 102, 130], start=180, end=360, fill=CIANO, width=7)


# cadeado: corpo arredondado + alca em arco
def cadeado(dd):
    dd.rounded_rectangle([28, 56, 100, 112], radius=10, outline=CIANO, width=7)
    dd.arc([44, 18, 84, 72], start=180, end=360, fill=CIANO, width=7)
    dd.ellipse([60, 76, 68, 84], fill=CIANO)


# olho: amendoa + pupila
def olho(dd):
    dd.arc([16, 36, 112, 92], start=0, end=180, fill=CIANO, width=7)
    dd.arc([16, 36, 112, 92], start=180, end=360, fill=CIANO, width=7)
    dd.ellipse([52, 52, 76, 76], fill=CIANO)


salvar_icone('icone_pessoa', pessoa)
salvar_icone('icone_cadeado', cadeado)
salvar_icone('icone_olho', olho)


# ---------------- 3. O PAINEL DE HUD ----------------
PL, PA = 2048, 1152
painel = Image.new('RGBA', (PL, PA), (0, 0, 0, 0))
dp = ImageDraw.Draw(painel)
M, R = 40, 34
dp.rounded_rectangle([M, M, PL - M, PA - M], radius=R, fill=(8, 14, 30, 232), outline=(60, 90, 140, 255), width=2)
linha = Image.new('RGBA', (PL, PA), (0, 0, 0, 0))
dl = ImageDraw.Draw(linha)
dl.rounded_rectangle([M, M, PL - M, PA - M], radius=R, outline=CIANO, width=3)
painel.alpha_composite(linha.filter(ImageFilter.GaussianBlur(10)))
painel.alpha_composite(linha)

# os COLCHETES de canto: e o que da a leitura de HUD no alvo do autor
B = 150
for (px, py, sx, sy) in ((M, M, 1, 1), (PL - M, M, -1, 1), (M, PA - M, 1, -1), (PL - M, PA - M, -1, -1)):
    dp.line([(px, py + sy * B), (px, py), (px + sx * B, py)], fill=CIANO, width=7)
    dp.line([(px + sx * 12, py + sy * B - sy * 12), (px + sx * 12, py + sy * 12), (px + sx * B - sx * 12, py + sy * 12)],
            fill=(0, 140, 180, 200), width=2)
painel.save(RAIZ + 'painel_hud_v001.png')

print('kit do login gerado: logo, tres icones e o painel de HUD com colchetes')
