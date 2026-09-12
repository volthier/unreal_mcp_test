Você é o agente técnico e artístico responsável por RECRIAR DO ZERO, dentro do Unreal Engine 5.8, a interface de login sci-fi/cyberpunk mostrada na imagem de referência fornecida nesta conversa.

OBJETIVO PRINCIPAL

Reproduza com altíssima fidelidade visual o PAINEL CENTRAL DE LOGIN da referência, incluindo:

- caixa/frame externo;
- profundidade e camadas do frame;
- área escura interna;
- logo superior;
- abas LOGIN / CADASTRO;
- campo E-mail ou Usuário;
- campo Senha;
- ícone de usuário/e-mail;
- ícone de cadeado;
- ícone mostrar/ocultar senha;
- checkbox "Lembrar de mim";
- link "Esqueceu a senha?";
- botão principal ENTRAR;
- divisor "ou continue com";
- botões sociais;
- Facebook;
- Instagram;
- Apple;
- Xbox;
- Google;
- Epic Games;
- Steam;
- texto inferior "CONECTE-SE A UM MUNDO MAIOR";
- glow;
- emissivos;
- bordas metálicas;
- reflexos;
- detalhes holográficos;
- microdetalhes tecnológicos;
- animações de idle;
- hover;
- focus;
- pressed;
- disabled;
- transições entre Login e Cadastro.

A implementação final deverá ser funcional no Unreal Engine 5.8, e não apenas uma imagem renderizada.

Use:

UNREAL ENGINE 5.8 + MCP
BLENDER + MCP
COMFYUI para geração/iteração de elementos 2D, máscaras, padrões e concept support.

Não substitua a UI funcional por uma única imagem.

============================================================
1. DIREÇÃO VISUAL
============================================================

O estilo deve ser:

futuristic cyberpunk game interface
sci-fi action game UI
high-tech robotic aesthetic
dark metallic interface
electric cyan lighting
subtle magenta highlights
Mega-Man-X-inspired visual language WITHOUT copiar diretamente assets existentes de terceiros
clean premium game UI
hard-surface sci-fi
holographic technology
layered glass/metal interface
high contrast
sharp geometric shapes
controlled bloom

Visualmente o painel deve passar a sensação de:

TECNOLOGIA
ROBÓTICA
ENERGIA
PRECISÃO
FUTURO
SISTEMA MILITAR AVANÇADO

Não fazer estética genérica de website.

Isto é uma interface de videogame AAA estilizada.

============================================================
2. COMPOSIÇÃO GERAL
============================================================

Crie um painel vertical central aproximadamente na proporção:

largura : altura ≈ 0.68 : 1

Referência aproximada:

420–520 px de largura lógica
680–780 px de altura lógica

mas construa responsivamente usando UMG.

O painel deverá permanecer perfeitamente centralizado.

Hierarquia visual:

[ FRAME EXTERNO ]
        |
        +-- LOGO
        |
        +-- LOGIN / CADASTRO
        |
        +-- EMAIL
        |
        +-- SENHA
        |
        +-- REMEMBER / FORGOT
        |
        +-- ENTRAR
        |
        +-- DIVISOR
        |
        +-- SOCIAL LOGIN
        |
        +-- STEAM
        |
        +-- TEXTO INFERIOR

Não comprimir verticalmente os elementos.

Use bastante espaço negativo para preservar legibilidade.

============================================================
3. FRAME PRINCIPAL
============================================================

O container deve parecer uma peça tecnológica construída fisicamente.

Não faça somente um retângulo 2D.

Estrutura visual em múltiplas camadas:

LAYER 01
shadow/background plate

LAYER 02
black/dark metallic backplate

LAYER 03
blue-black inner plate

LAYER 04
glass/acrylic translucent surface

LAYER 05
metallic outer armor

LAYER 06
cyan emissive trims

LAYER 07
small magenta energy accents

LAYER 08
specular scratches / imperfections

LAYER 09
UI content

Geometria:

cantos cortados;
chanfros;
pequenos degraus geométricos;
placas sobrepostas;
linhas diagonais;
detalhes mecânicos nas quinas;
topo e base ligeiramente mais complexos.

Evitar:

rounded rectangle moderno;
glassmorphism genérico;
bordas circulares;
formas excessivamente suaves.

A linguagem deve ser HARD SURFACE SCI-FI.

============================================================
4. BLENDER — FRAME 3D
============================================================

Utilize Blender MCP para construir uma versão modular do frame.

Crie:

SM_LoginFrame_Outer
SM_LoginFrame_Inner
SM_LoginFrame_Glass
SM_LoginFrame_CornerTL
SM_LoginFrame_CornerTR
SM_LoginFrame_CornerBL
SM_LoginFrame_CornerBR
SM_LoginFrame_TopTrim
SM_LoginFrame_BottomTrim

Modele usando:

hard-surface modeling;
bevel;
weighted normals;
controlled chamfers;
small recessed channels;
panel cuts.

Profundidade baixa.

O frame deve parecer um objeto 3D fino, semelhante a um console/holographic terminal.

Crie UVs adequadas.

Materiais separados:

MI_Login_Metal
MI_Login_DarkMetal
MI_Login_Glass
MI_Login_CyanEmissive
MI_Login_MagentaEmissive

Não adicionar detalhes geométricos desnecessários que aumentem draw calls.

Se determinadas peças forem mais eficientes como imagens 9-slice dentro do UMG, faça bake/render dessas partes.

============================================================
5. MATERIAIS
============================================================

METAL EXTERNO:

Base Color:
#071522
até
#142B3D

Metallic:
0.75 – 1.0

Roughness:
0.18 – 0.32

Normal:
micro brushed metal

Detalhes muito discretos de desgaste.

INTERIOR:

#050B14
#07121D
#081827

Roughness aproximadamente:
0.3–0.45

GLASS:

azul escuro quase transparente

Opacity:
0.10–0.22

Fresnel leve.

REFLECTION muito sutil.

============================================================
6. PALETA
============================================================

PRIMARY CYAN

#00E5FF

ELECTRIC BLUE

#008CFF

BRIGHT CYAN

#43F4FF

MAGENTA

#FF2ED1

VIOLET

#8A2BE2

DEEP BLUE

#0A1B33

ALMOST BLACK

#050B14

LIGHT TEXT

#EAF2FF

SECONDARY TEXT

#94A3B8

Use magenta apenas como accent.

A UI deve ser predominantemente:

BLACK / DARK BLUE + CYAN.

============================================================
7. EMISSIVE
============================================================

Crie materiais emissivos parametrizados.

Parâmetros:

GlowColor
GlowIntensity
PulseSpeed
PulseAmplitude

Default cyan emissive:

color = #00E5FF

Intensity base aproximadamente:
3–7

Hover:
8–12

Pressed:
4–6

Use Bloom do Unreal.

Não fazer bloom exagerado a ponto de perder detalhes.

============================================================
8. LOGO
============================================================

No topo:

AETHER FORGE X
PROTOCOL ZERO

Criar logo sci-fi metálico angular.

"AETHER FORGE"

metal claro / prata / azul.

"X"

grande;
angular;
energia cyan + magenta;
alto contraste;
elemento mais energético da logo.

Subtítulo:

PROTOCOL ZERO

tracking alto.

O logo não deverá competir com os campos.

============================================================
9. TIPOGRAFIA
============================================================

Usar fonte futurista geométrica legível.

Preferência:

Orbitron para títulos

ou fonte semelhante.

Para UI:

Exo 2
Rajdhani
Inter

Hierarquia aproximada:

LOGO:
display/custom

LOGIN / CADASTRO:
14–18 px lógico

input:
15–18

button:
18–22 bold

secondary:
12–14

footer:
9–12
letter spacing elevado.

Não usar fontes decorativas nos inputs.

============================================================
10. LOGIN / CADASTRO
============================================================

Criar selector horizontal.

[ LOGIN ][ CADASTRO ]

LOGIN ativo:

cyan glow;
blue gradient;
bottom/inner highlight;
texto branco.

CADASTRO inativo:

dark blue;
thin cyan border;
texto cinza claro.

Transição:

0.15–0.25 segundos.

Ao trocar:

animated glow slide.

============================================================
11. INPUTS
============================================================

Criar reusable widget:

WBP_LoginInput

Estados:

Normal
Hover
Focused
Invalid
Disabled

DIMENSÕES aproximadas:

altura:
46–54 px

border:
1–2 px

inner padding:
14–18 px

background:

#071321

border normal:

#17577D

hover:

#00AEEA

focused:

#00E5FF

Crie inner glow discreto.

EMAIL:

ícone esquerdo:
user/mail outline icon

placeholder:

"E-mail ou Usuário"

SENHA:

ícone esquerdo:
cadeado

ícone direito:
olho

placeholder:

"Senha"

Eye icon deve alternar:

Password
Normal text

============================================================
12. CHECKBOX
============================================================

Criar checkbox customizado.

Não utilizar aparência default do Unreal.

Square box aproximadamente:

18–22 px.

Border:
cyan-gray.

Checked:

cyan emissive;
checkmark branco.

Texto:

"Lembrar de mim"

============================================================
13. ESQUECEU A SENHA
============================================================

Alinhado à direita.

Texto:

"Esqueceu a senha?"

Cor:

#00DDF5

Hover:

brilho cyan + underline.

============================================================
14. BOTÃO ENTRAR
============================================================

Este é o elemento de maior destaque funcional.

Texto:

ENTRAR

Frame:

hexagonal / angular;
laterais recortadas;
cyan emissive border;
blue inner gradient;
dark core.

Adicionar pequenos chevrons / arrows nas laterais.

NORMAL:

glow médio.

HOVER:

glow aumenta;
inner gradient anima;
pequena sweep light atravessa o botão.

PRESSED:

scale ≈ 0.98
glow reduz momentaneamente.

RELEASE:

pequeno energy pulse.

Não usar rounded rectangle.

============================================================
15. DIVISOR
============================================================

linha horizontal esquerda

texto:

"ou continue com"

linha horizontal direita

Linhas:

dark cyan / grey blue.

Texto:

secondary light blue/white.

============================================================
16. SOCIAL LOGIN
============================================================

Crie reusable widget:

WBP_SocialLoginButton

Layout principal:

ROW 1

Facebook
Instagram
Apple

ROW 2

Xbox
Google
Epic Games

ROW 3

Steam

Steam centralizado e aproximadamente duas colunas de largura ou proporcional ao design da referência.

Cada botão:

dark background;
thin cyan/blue border;
icone à esquerda;
provider name;
small hover glow.

Não exagerar cores.

As marcas podem manter suas cores oficiais nos ícones, enquanto o container permanece coerente com o sistema visual.

Normal:

#071321

Hover:

border cyan
slight blue interior glow

Pressed:

background mais escuro.

Use ícones em alta qualidade e preserve proporção.

============================================================
17. FOOTER
============================================================

Texto:

CONECTE-SE A UM MUNDO MAIOR

uppercase

letter spacing alto

cyan muito suave / steel blue.

Centralizado.

============================================================
18. UMG — ARQUITETURA
============================================================

Criar:

WBP_LoginScreen

Estrutura sugerida:

CanvasPanel
 └── Overlay_LoginRoot
      ├── BackgroundShadow
      ├── FrameDecoration
      ├── FrameGlass
      ├── FrameGlow
      └── SizeBox_Content
           └── VerticalBox
                ├── Logo
                ├── Spacer
                ├── WBP_LoginTabs
                ├── Spacer
                ├── WBP_EmailInput
                ├── WBP_PasswordInput
                ├── RememberForgotRow
                ├── Spacer
                ├── WBP_PrimaryButton
                ├── Divider
                ├── SocialGrid
                ├── SteamButton
                └── Footer

Criar reusable components:

WBP_LoginInput
WBP_PrimaryButton
WBP_SecondaryButton
WBP_LoginTabs
WBP_SocialLoginButton
WBP_CheckBox
WBP_LoginDivider

============================================================
19. RESPONSIVIDADE
============================================================

A interface precisa funcionar corretamente em:

1920×1080

2560×1440

3440×1440 ultrawide

3840×2160

Use:

Anchors
ScaleBox quando adequado
SizeBox
VerticalBox
HorizontalBox
GridPanel
Overlay

Evitar posições absolutas sempre que possível.

Safe Zone compatible.

============================================================
20. UI MATERIALS
============================================================

Criar:

M_UI_CyberGlow
M_UI_Holographic
M_UI_Scanline
M_UI_EnergySweep
M_UI_Glass
M_UI_Noise

Adicionar parâmetros:

Time
Intensity
Color
Opacity
ScanSpeed
NoiseAmount
SweepPosition

============================================================
21. MICROANIMAÇÃO DO FRAME
============================================================

Frame idle:

cyan emissive pulse extremamente sutil.

Período:

2–4 segundos.

Alguns detalhes de energia magenta podem percorrer os cantos ocasionalmente.

Nada piscando rapidamente.

============================================================
22. SCANLINES
============================================================

Adicionar scanlines MUITO discretas na superfície interna.

Opacity aproximada:

0.02–0.05

Scroll extremamente lento.

Não prejudicar legibilidade.

============================================================
23. HOLOGRAPHIC NOISE
============================================================

Adicionar noise eletrônico muito discreto.

Intensity:

0.01–0.03.

Pode variar lentamente.

============================================================
24. COMFYUI
============================================================

Use ComfyUI SOMENTE como suporte artístico.

Pode gerar:

surface patterns;
micro scratches;
sci-fi panels;
holographic noise;
emissive masks;
UI concept variations;
logo explorations;
energy patterns;
metal roughness masks.

Não gerar toda a UI como uma imagem única.

A interface final deve continuar sendo UMG funcional.

Gerar assets preferencialmente em:

PNG
transparent background
2K ou 4K somente quando necessário.

Depois otimizar.

============================================================
25. TEXTURE SET
============================================================

Criar se necessário:

T_LoginFrame_BaseColor
T_LoginFrame_Normal
T_LoginFrame_ORM
T_LoginFrame_EmissiveMask
T_LoginFrame_GlassMask
T_UI_Noise
T_UI_Scanlines
T_UI_EnergySweep
T_UI_Icons

Considerar atlas para pequenos elementos.

============================================================
26. ÍCONES
============================================================

Todos os ícones UI comuns devem compartilhar:

mesma espessura de linha;
mesma escala visual;
mesmo padding.

Criar:

email/user
lock
eye
eye-off
checkbox
arrow-left/right

Preferência:

vector ou SDF.

============================================================
27. ORGANIZAÇÃO DE ASSETS
============================================================

/Game/UI/Login/

/Widgets
    WBP_LoginScreen
    WBP_LoginInput
    WBP_LoginTabs
    WBP_PrimaryButton
    WBP_SocialLoginButton
    WBP_CheckBox
    WBP_LoginDivider

/Textures

/Icons

/Materials

/MaterialInstances

/Fonts

/Meshes

/Animations

/Data

============================================================
28. STYLE SYSTEM
============================================================

Não colocar todas as cores diretamente nos widgets.

Criar estrutura central de estilo:

LoginPrimaryColor
LoginSecondaryColor
LoginBackgroundColor
LoginBorderColor
LoginTextPrimary
LoginTextSecondary
LoginGlowIntensity
LoginCornerSize
LoginPadding
LoginAnimationSpeed

Permitir alteração posterior sem editar dezenas de widgets.

============================================================
29. INTERAÇÃO
============================================================

Implementar:

mouse

keyboard

controller/gamepad

Navegação:

Email
↓
Password
↓
Remember
↓
Login
↓
Social Providers

Gamepad focus deve possuir outline/glow específico.

ENTER / A confirma.

ESC / B retorna quando aplicável.

TAB alterna campos.

============================================================
30. LOGIN STATE MACHINE
============================================================

Estados:

Idle
EnteringCredentials
Authenticating
Success
Error

AUTHENTICATING:

botão ENTRAR passa para loading.

Adicionar pequeno loader holográfico.

ERROR:

border dos campos envolvidos muda para:

#FF6688

Mensagem discreta abaixo.

Não usar popup invasivo.

SUCCESS:

cyan flash curto

aproximadamente 0.25–0.4 s.

============================================================
31. PERFORMANCE
============================================================

Evitar:

tick Blueprint desnecessário;
material dinâmico excessivo;
muitos widgets sobrepostos;
texturas gigantes;
blur caro em toda tela;
muitos draw calls.

Usar animations/timers/material time.

Invalidation quando adequado.

Retainer Box somente se oferecer benefício real.

============================================================
32. BLENDER → UNREAL
============================================================

Exportar meshes como FBX ou formato adequado ao pipeline existente.

Aplicar:

correct scale;
transforms;
normals;
UVs.

Naming Unreal-friendly.

Pivot central apropriado.

Se o frame 3D não produzir benefício perceptível comparado a um sistema 9-slice renderizado, utilize o Blender para gerar o frame high-quality e faça bake/render em peças modulares para utilização eficiente no UMG.

============================================================
33. TARGET VISUAL
============================================================

A aparência final deverá ser muito próxima da referência:

painel estreito vertical;
grande logo superior;
predominância azul escuro;
borda cyan tecnológica;
pequenos accents magenta;
inputs escuros;
contornos cyan discretos;
grande botão ENTRAR neon;
social buttons organizados em grid;
Steam separado abaixo;
rodapé tecnológico;
simetria forte;
sensação premium;
alto contraste;
excelente legibilidade.

NÃO reinterpretar a estrutura principal.

NÃO transformar em menu horizontal.

NÃO mudar para estética minimalista.

NÃO fazer estilo mobile.

NÃO usar rounded cards genéricos.

============================================================
34. QUALIDADE AAA
============================================================

O resultado deverá parecer uma tela final de videogame, não um protótipo de UI.

Observe atentamente:

alignment;
padding;
baseline;
spacing;
visual hierarchy;
consistent borders;
consistent bevels;
consistent icon sizes;
consistent typography;
controlled glow;
sharp geometry.

Utilize grid de alinhamento.

Nenhum elemento deve parecer colocado manualmente sem regra.

============================================================
35. PROCEDIMENTO DO AGENTE
============================================================

ANTES DE MODIFICAR O PROJETO:

1. Analise a imagem de referência.
2. Inspecione a estrutura atual do projeto Unreal.
3. Localize qualquer UI/login existente.
4. Não destrua assets existentes sem necessidade.
5. Crie os novos assets de maneira organizada.

DEPOIS:

Blender:
construir frame e elementos necessários.

ComfyUI:
gerar somente textures/masks/concept support necessários.

Unreal MCP:
importar assets;
criar materials;
criar Material Instances;
criar widgets;
montar hierarchy;
configurar anchors;
configurar interação;
configurar animações;
configurar states;
configurar login screen.

============================================================
36. VALIDAÇÃO ITERATIVA OBRIGATÓRIA
============================================================

Não considere a tarefa terminada depois de simplesmente criar os assets.

Execute a tela.

Capture screenshot.

Compare visualmente com a referência.

Procure diferenças em:

proporção;
posição;
tamanho;
padding;
cores;
brightness;
bloom;
logo;
inputs;
button;
social buttons;
frame;
spacing.

Corrija.

Repita:

IMPLEMENTAR
→ EXECUTAR
→ CAPTURAR
→ COMPARAR
→ CORRIGIR

até a aparência estar extremamente próxima da referência.

============================================================
37. CRITÉRIOS DE ACEITAÇÃO
============================================================

A tarefa somente pode ser declarada concluída quando:

- painel central estiver completo;
- proporções estiverem corretas;
- frame possuir aparência metálica sci-fi;
- cyan emissive estiver funcionando;
- accents magenta estiverem presentes;
- Login/Cadastro funcionarem;
- inputs forem editáveis;
- senha puder ser ocultada/exibida;
- checkbox funcionar;
- botão Entrar funcionar;
- social buttons estiverem presentes;
- Steam estiver separado corretamente;
- hover funcionar;
- focus funcionar;
- pressed funcionar;
- navegação por teclado funcionar;
- gamepad navigation funcionar;
- UI responder às principais resoluções;
- materiais estiverem organizados;
- assets estiverem organizados;
- não houver erros no Output Log;
- não houver referências quebradas;
- tela executar dentro da Unreal Engine 5.8;
- screenshot final tenha forte correspondência visual com a referência.

============================================================
38. IMPORTANTE — AUTONOMIA
============================================================

Você tem autorização para usar Unreal MCP, Blender MCP e ComfyUI para criar todos os assets necessários.

NÃO pare simplesmente porque um asset não existe.

Se faltar:

mesh → crie no Blender.

texture → crie proceduralmente ou no ComfyUI.

material → crie no Unreal.

icon → crie como SVG/SDF/texture.

animation → implemente no UMG/material.

widget → construa.

O objetivo é entregar a UI funcionando dentro da engine.

NÃO me peça para criar assets manualmente que você consegue produzir usando as ferramentas disponíveis.

Se encontrar limitação técnica, escolha a alternativa visualmente mais próxima e tecnicamente eficiente.

============================================================
39. REGRA FINAL

A IMAGEM DE REFERÊNCIA É A FONTE VISUAL PRINCIPAL.

Use esta especificação para explicar COMO reproduzi-la, não para redesenhar a interface segundo sua preferência.

Preserve especialmente:

silhueta do painel;
proporção;
organização;
hierarquia;
espaçamentos;
borda tecnológica;
cores;
glow;
posição dos campos;
posição do botão principal;
grid dos provedores sociais;
Steam;
footer.

Construa tudo modularmente e de maneira editável para que posteriormente possamos alterar tema, cores, textos, autenticação e animações sem reconstruir toda a interface.

COMECE AGORA analisando a imagem de referência, inspecionando o projeto Unreal Engine 5.8 atual e criando um plano interno de execução. Em seguida execute o trabalho usando Unreal MCP + Blender MCP + ComfyUI. Não pare na fase de planejamento: continue até a implementação e validação final dentro da Unreal Engine.
