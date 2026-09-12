# AETHER FORGE X — PROTOCOL ZERO
# CHARACTER SELECTION SCREEN
# MASTER IMPLEMENTATION PROMPT V2
# Unreal Engine 5.8 + Unreal MCP + Blender MCP + ComfyUI

Você é o agente técnico, artístico e de implementação responsável por RECRIAR DO ZERO dentro do Unreal Engine 5.8 a tela de SELEÇÃO DE PERSONAGEM de AETHER FORGE X — PROTOCOL ZERO.

Você possui acesso a:

- Unreal Engine 5.8 via MCP
- Blender via MCP
- ComfyUI
- UMG
- Blueprint
- Niagara
- Material Editor
- PCG quando pertinente
- ferramentas de importação/exportação
- automação disponível no ambiente

A tarefa NÃO consiste em simplesmente produzir uma concept art.

O resultado final deve ser uma TELA FUNCIONAL e EXECUTÁVEL dentro do Unreal Engine 5.8.

======================================================================
0. REGRA DE PRIORIDADE DAS REFERÊNCIAS
======================================================================

Existem três referências visuais fornecidas:

1. TSP_Tela_Selecao_Personagem_art_final.png
2. TSP_guia_conceitual_part1.png
3. TSP_guia_conceitual_part2.png

A PRIORIDADE É:

TSP_Tela_Selecao_Personagem_art_final.png
        ↓
REFERÊNCIA VISUAL CANÔNICA

TSP_guia_conceitual_part1.png
TSP_guia_conceitual_part2.png
        ↓
DOCUMENTAÇÃO DE CONSTRUÇÃO E DIREÇÃO ARTÍSTICA

REGRA ABSOLUTA:

Se existir qualquer diferença entre a arte final e as pranchas conceituais, A ARTE FINAL TEM PRIORIDADE VISUAL.

As pranchas NÃO representam outra tela.

Elas explicam:

- materiais;
- assets;
- estados;
- composição;
- personagens;
- iluminação;
- paleta;
- comportamento;
- implementação;
- pipeline.

NÃO reproduza as caixas, números, chamadas ou anotações das pranchas conceituais dentro do jogo.

======================================================================
1. PIXEL-COMPOSITION LOCK
======================================================================

A composição visual da arte final deve ser tratada como BLOQUEADA.

Preservar:

- silhueta geral;
- posição relativa dos elementos;
- escala relativa;
- espaçamento;
- hierarquia;
- proporção dos cards;
- largura do painel;
- posição do logo;
- posição do título;
- posição dos botões;
- tamanho relativo dos personagens;
- distribuição da cidade;
- tempestade;
- enxames;
- obeliscos;
- foreground;
- atmosfera.

NÃO redesenhar a interface segundo preferências próprias.

NÃO transformar a UI em dashboard moderno.

NÃO transformar em layout web.

NÃO aplicar glassmorphism genérico.

NÃO aumentar arbitrariamente padding ou gaps.

NÃO substituir hard-surface por rounded rectangles.

NÃO alterar a hierarquia visual sem necessidade técnica comprovada.

A arte final original possui composição vertical aproximadamente 941 × 1672.

Use esta proporção como referência de enquadramento.

Isso NÃO significa que a UI final deverá usar coordenadas absolutas.

A implementação final deve ser responsiva.

======================================================================
2. OBJETIVO VISUAL
======================================================================

Construir uma tela AAA de seleção de personagem que combine:

- cyberpunk;
- sci-fi;
- robótica;
- post-human civilization;
- anime sci-fi action game;
- mecha;
- megacidade;
- nanotecnologia;
- tempestade energética;
- hard-surface UI;
- holographic military technology.

A inspiração visual pode lembrar a energia e clareza de jogos de ação japoneses sci-fi como Mega Man X/X8, mas todos os assets devem possuir identidade original.

Sensações principais:

TECNOLOGIA
MISTÉRIO
PERIGO
EVOLUÇÃO
ESCALA
ESPERANÇA EM UM MUNDO EM RUÍNAS

======================================================================
3. REGRA CROMÁTICA DO MUNDO
======================================================================

Existem três linguagens de cor principais.

CYAN / BLUE:
tecnologia;
interface;
sistema;
energia aliada;
KAEL-7;
seleção ativa.

MAGENTA / PURPLE:
tempestade;
nano-robôs;
ameaça;
LYRA;
anomalia.

ORANGE:
força;
industrial;
defesa;
BRONT.

Não misture essas linguagens indiscriminadamente.

======================================================================
4. PALETA CANÔNICA
======================================================================

CYAN ELÉTRICO
#00E5FF

MAGENTA NEON
#FF00D4

VIOLETA TEMPESTADE
#8A2BFF

AZUL PROFUNDO
#081A3C

METAL GAME
#2B2F3A

BRANCO DESTAQUE
#FFFFFF

BLACK BLUE
#020713

DARK NAVY
#050B14

SECONDARY BLUE
#0B2944

SECONDARY TEXT
#A5B8D4

BRONT ORANGE
#FF9B21

ERROR
#FF526B

======================================================================
5. BACKGROUND DO MUNDO
======================================================================

O cenário deve ocupar toda a tela atrás da interface.

A UI existe sobre um mundo 3D real.

NÃO usar apenas uma imagem estática como background final.

Criar uma megacidade colossal parcialmente escondida por nuvens.

Elementos obrigatórios:

- torres extremamente altas;
- megaestruturas verticais;
- megacidade em diferentes níveis;
- plataformas suspensas;
- pontes;
- estruturas industriais;
- estruturas em ruínas;
- torres parcialmente cobertas por nuvens;
- arquitetura angular;
- iluminação magenta;
- iluminação cyan;
- grandes espaços verticais;
- profundidade atmosférica.

A cidade precisa parecer gigantesca.

======================================================================
6. CIDADE CENTRAL SUPERIOR
======================================================================

No topo da composição existe uma enorme cidade/fortaleza suspensa.

Características:

- torre central monumental;
- várias torres secundárias;
- estruturas altas e extremamente finas;
- grandes anéis tecnológicos;
- plataformas circulares;
- iluminação magenta intensa;
- pontos cyan;
- metal quase preto;
- cidade parcialmente escondida pela tempestade.

A estrutura não deve parecer uma cidade terrestre convencional.

É uma megacidade tecnológica vertical.

======================================================================
7. TEMPESTADE DE NANO-ROBÔS
======================================================================

A tempestade é um elemento narrativo principal.

Ela mistura:

- volumetric clouds;
- smoke;
- fog;
- bilhões de nano-robôs;
- partículas metálicas;
- micro-drones;
- fragmentos tecnológicos.

Ela deve parecer simultaneamente:

NUVEM
ENXAME
MÁQUINA
ENTIDADE VIVA

Criar em Niagara e/ou volumetric materials.

Os nano-robôs não devem parecer simples partículas aleatórias.

Usar comportamento semelhante a:

BOIDS / FLOCKING

com conceitos de:

Separation
Alignment
Cohesion
Vortex Attraction
Flow Field
Spline Guidance

Os nanos devem:

- formar enxames;
- se separar;
- reagrupar;
- criar vórtices;
- atravessar a cidade;
- desaparecer dentro da tempestade;
- emergir novamente.

Evitar movimento uniforme.

======================================================================
8. RAIOS
======================================================================

A tempestade contém raios constantes porém imprevisíveis.

Cores:

magenta;
violeta;
rosa elétrico;
núcleo quase branco.

Cada raio deve produzir:

flash;
bloom;
light contribution;
illumination on clouds;
illumination on buildings;
brief exposure change;
reflections on metal.

Não utilizar sequência perfeitamente periódica.

Randomizar:

position;
length;
branching;
intensity;
delay.

======================================================================
9. ESTRUTURAS FLUTUANTES
======================================================================

Existem estruturas verticais flutuantes ao redor da cidade.

Criar assets modulares:

SM_FloatingObelisk_A
SM_FloatingObelisk_B
SM_FloatingObelisk_C

Características:

dark metal;
hard-surface;
vertical;
thin;
sharp;
magenta internal core;
cyan micro accents;
glowing vents.

Algumas aparecem muito próximas.

Outras aparecem distantes.

Isso reforça profundidade.

======================================================================
10. FOREGROUND INFERIOR
======================================================================

Na parte inferior esquerda há uma grande plataforma industrial.

Elementos:

- corrimões;
- tubos;
- antenas;
- painéis;
- cabos;
- consoles;
- iluminação magenta;
- metal quase preto.

Sobre a plataforma deve existir uma pequena figura humana observando a cidade.

A FIGURA É IMPORTANTE.

Ela fornece referência de escala.

Ao visualizar a figura, o jogador precisa perceber imediatamente que a megacidade é colossal.

======================================================================
11. CENÁRIO ABAIXO DO PAINEL
======================================================================

A cidade continua abaixo da interface.

O mundo inferior NÃO é outra imagem.

Deve ser fisicamente e artisticamente conectado ao cenário superior.

Mostrar:

- torres;
- ruínas;
- plataformas;
- nuvens;
- magenta lights;
- nano swarms;
- distant buildings.

A interface parece flutuar sobre esse mundo.

======================================================================
12. TEXTOS AMBIENTAIS
======================================================================

Adicionar discretamente alguns textos ao cenário.

SUPERIOR ESQUERDO:

MAIS
QUE UM
JOGO.
UM NOVO
AMANHÃ.

SUPERIOR DIREITO:

A
HUMANIDADE
AINDA
EXISTE.

INFERIOR DIREITO:

NANOS
NUNCA
ESQUECEM

Configuração:

uppercase;
thin font;
high tracking;
blue-white;
low opacity.

Não competir com a UI.

======================================================================
13. LOGO
======================================================================

Na região superior central:

AETHER FORGE X

AETHER FORGE:

metallic silver;
blue reflections;
sharp geometry;
beveled;
hard-surface typography.

O X:

muito maior;
asymmetric;
cyan na região esquerda;
magenta na região direita;
bright emissive;
metallic border;
controlled bloom.

Abaixo:

PROTOCOLO ZERO

uppercase;
high letter spacing;
white/magenta.

IMPORTANTE:

O logo principal é um elemento SEPARADO do título da UI.

NÃO fundir:

AETHER FORGE X

com:

SELEÇÃO DE PERSONAGEM.

======================================================================
14. PAINEL PRINCIPAL
======================================================================

Abaixo do logo existe o painel de seleção.

O painel deve ocupar aproximadamente 90% da largura útil da composição original.

Não use esse número como coordenada rígida.

Use como referência de proporção.

O painel é:

dark;
metallic;
hard-surface;
layered;
technological.

Camadas:

Shadow
OuterMetal
InnerMetal
Glass
EnergyTrim
Content

Utilizar:

bevels;
cut corners;
angular sections;
recessed channels;
small mechanical details;
cyan edge lights;
rare magenta accents.

Evitar arredondamento moderno.

======================================================================
15. ESTRUTURA DO PAINEL
======================================================================

Organização:

LOGO EXTERNO

↓

PANEL

SELEÇÃO DE PERSONAGEM

MESMOS SONHOS. NOVAS REALIDADES.

↓

Personagens da conta                3 / 4 espaços utilizados

↓

KAEL-7 | LYRA | BRONT | CRIAR NOVO

↓

ENTRAR NO MUNDO | CRIAR NOVO | APAGAR | VOLTAR

↓

CONEXÃO COM UM MUNDO MAIOR

======================================================================
16. HEADER INTERNO
======================================================================

Título:

SELEÇÃO DE PERSONAGEM

uppercase;
white;
bold;
sci-fi;
high readability.

Abaixo:

— MESMOS SONHOS. NOVAS REALIDADES. —

Small text;
cyan-muted;
high tracking.

======================================================================
17. INFORMAÇÕES DA CONTA
======================================================================

ESQUERDA:

user/account outline icon

Personagens da conta

DIREITA:

3 / 4 espaços utilizados

Usar font de UI, não display font.

======================================================================
18. CHARACTER GRID
======================================================================

Quatro cards.

Gap entre eles:

pequeno;
uniforme;
compacto.

IMPORTANTE:

NÃO usar gaps grandes típicos de dashboard/web.

A composição deve parecer densa e integrada.

Ordem:

KAEL-7
LYRA
BRONT
CRIAR NOVO

======================================================================
19. COMPONENTE WBP_CHARACTER_CARD
======================================================================

Criar widget reutilizável:

WBP_CharacterCard

Estados:

Normal
Hover
Selected
Locked
Disabled
Empty

IMPORTANTE:

Locked e Disabled NÃO significam a mesma coisa.

LOCKED:

personagem/slot existente mas indisponível;
pode mostrar silhouette;
lock icon.

DISABLED:

componente temporariamente não interativo.

======================================================================
20. KAEL-7
======================================================================

Nome:

KAEL-7

Classe:

RANGER

Nível:

NÍVEL 42

Status:

SELECIONADO

KAEL-7 é o personagem selecionado por padrão.

Visual obrigatório:

male humanoid android;
athletic armored body;
black;
gunmetal;
dark navy;
cyan energy.

Helmet:

fully closed;
dark faceplate;
no visible human face.

Visor:

forte cyan.

Criar dois pontos cyan muito intensos na região facial.

Armadura:

segmented;
advanced;
sharp;
tactical;
high-tech.

Possui:

black asymmetric scarf/cape.

O tecido deve cair sobre ombro e costas.

Arma:

long rifle;
positioned vertically behind shoulder/back.

Silhueta:

atlética;
blindada;
mais fina que BRONT;
mais pesada que LYRA.

Lighting:

strong cyan rim light.

Background do card:

blue;
cyan;
storm;
fog.

Não simplificar KAEL-7 para um soldado genérico.

======================================================================
21. ESTADO SELECTED DE KAEL-7
======================================================================

O card selecionado não possui apenas glow.

Ele deve ter:

strong cyan external halo;
cyan outer frame;
inner cyan line;
mechanical illuminated corners;
vertical energy highlights;
cyan spill over portrait;
subtle animated border;
bright bottom bar.

Na parte inferior:

SELECIONADO

Usar barra cyan sólida.

Texto sobre a barra:

dark blue / almost black.

A seleção precisa ser instantaneamente reconhecível.

======================================================================
22. LYRA
======================================================================

Nome:

LYRA

Classe:

ASSASSINA

Nível:

NÍVEL 37

Visual:

female cybernetic assassin.

Proporção:

athletic;
slender;
agile;
narrow waist.

Cabelo:

white/silver;
pink-purple reflections;
long;
high ponytail.

Pose:

approximately 3/4 turned away;
body partially facing away;
head looking back toward viewer.

Armor:

black glossy;
dark violet;
sleek;
high-tech;
magenta emissive lines.

Lighting:

pink;
violet;
magenta rim.

Card accent:

magenta.

Classe:

ASSASSINA

magenta.

Ícone da classe:

sharp futuristic assassin symbol.

======================================================================
23. BRONT
======================================================================

Nome:

BRONT

Classe:

DEFENSOR

Nível:

NÍVEL 28

Visual:

massive heavy mechanized defender.

BRONT deve ser VISUALMENTE MUITO MAIOR que KAEL-7 e LYRA.

Do not normalize the three character body proportions.

BRONT:

very wide torso;
huge shoulders;
large arms;
small relative head;
industrial armor;
heavy mechanical components;
square mechanical modules.

Colors:

gunmetal;
black;
dark gray;
orange emissive.

Card accent:

orange.

A composição pode quase comprimir o corpo dele dentro do card.

Isso comunica:

FORÇA
PESO
DEFESA
ESTABILIDADE

======================================================================
24. SLOT CRIAR NOVO
======================================================================

O quarto slot é deliberadamente vazio.

NÃO colocar personagem fantasma detalhado.

Background:

extremely dark navy;
faint city silhouette;
subtle fog.

Centro:

octagonal technological frame.

Dentro:

+

white core;
cyan glow.

Texto:

CRIAR NOVO
PERSONAGEM

Depois:

small horizontal divider.

Depois:

FORJE SUA
PRÓPRIA LENDA

Depois:

small divider.

O espaço vazio é parte importante do design.

======================================================================
25. ESTADOS VISUAIS DOS CARDS
======================================================================

NORMAL:

dark border;
subtle blue.

HOVER:

slightly brighter border;
small scale increase;
inner light.

SELECTED:

strong cyan;
animated border;
bottom status bar.

LOCKED:

dark silhouette;
desaturation;
lock icon;
reduced brightness.

EMPTY:

create-new layout.

DISABLED:

reduced opacity;
no interaction.

Hover scale:

aproximadamente:

1.00 → 1.015

Transition:

150–250 ms.

======================================================================
26. CHARACTER PROPORTION RULE
======================================================================

As diferenças de silhueta são fundamentais.

KAEL-7:
medium/athletic armored.

LYRA:
slim/agile.

BRONT:
huge/heavy.

NÃO reutilizar o mesmo corpo base visual sem compensar proporções.

Eles precisam ser reconhecíveis apenas pela silhueta.

======================================================================
27. BOTÕES INFERIORES
======================================================================

Quatro botões:

ENTRAR NO MUNDO
CRIAR NOVO
APAGAR
VOLTAR

Mesma baseline.

Mesma altura.

ENTRAR NO MUNDO deve ser maior horizontalmente.

======================================================================
28. ENTRAR NO MUNDO
======================================================================

Texto:

ENTRAR NO MUNDO

Na esquerda:

<<

ou dois pequenos chevrons equivalentes.

Frame:

angular;
hard-surface;
asymmetric;
bright cyan.

NORMAL:

medium cyan glow.

HOVER:

strong glow;
energy sweep.

PRESSED:

scale ~0.98;
temporary glow decrease.

RELEASE:

brief energy pulse.

Este é o CTA principal.

======================================================================
29. CRIAR NOVO BUTTON
======================================================================

Ícone:

+

Texto:

CRIAR NOVO

Border:

thin cyan.

Background:

dark transparent blue.

======================================================================
30. APAGAR
======================================================================

Ícone:

trash/bin.

Texto:

APAGAR

Normal:

neutral dark/cyan.

NÃO mostrar vermelho constantemente.

Red only for:

hover warning;
confirmation;
danger state.

Ao clicar:

abrir confirmation modal.

Exemplo:

Deseja realmente apagar KAEL-7?

Não executar imediatamente.

======================================================================
31. VOLTAR
======================================================================

Ícone:

left arrow / chevron.

Texto:

VOLTAR

======================================================================
32. FOOTER
======================================================================

Na base:

—  CONEXÃO COM UM MUNDO MAIOR  —

Small uppercase typography.

High tracking.

Muted cyan.

======================================================================
33. PERSONAGEM BLOQUEADO
======================================================================

A documentação conceitual também define estado LOCKED.

Preparar esse estado mesmo que os três personagens atuais estejam disponíveis.

Locked card:

dark silhouette;
reduced saturation;
lock icon;
name hidden or replaced;
non-selectable;
potential tooltip.

Não confundir com Disable geral de UI.

======================================================================
34. DATA-DRIVEN DESIGN
======================================================================

NÃO hardcode KAEL-7, LYRA e BRONT dentro do layout.

Criar:

FCharacterSelectionData

Campos recomendados:

CharacterId
CharacterName
CharacterClass
CharacterLevel
Portrait
ClassIcon
PrimaryColor
SecondaryColor
SkeletalMesh
ActorClass
CharacterPreviewActor
LastPlayed
IsAvailable
IsLocked
SaveSlot
AccountCharacterId

Criar:

DT_CharacterSelection

ou arquitetura equivalente.

Exemplos:

KAEL7:
Name = KAEL-7
Class = Ranger
Level = 42
Color = Cyan

LYRA:
Name = LYRA
Class = Assassina
Level = 37
Color = Magenta

BRONT:
Name = BRONT
Class = Defensor
Level = 28
Color = Orange

======================================================================
35. ACCOUNT / SAVE INTEGRATION
======================================================================

Character list must be populated from account/save data rather than existing only as designer-time cards.

Criar interface preparada para:

Account System
Save Game
Backend API
Game Instance

Fluxo:

Account Login
→ Fetch Character List
→ Populate Character Cards
→ Select Character
→ Enter World

Se backend ainda não existir, criar uma camada mock/data provider substituível.

Não amarrar os widgets diretamente a dados mockados.

======================================================================
36. UMG ARCHITECTURE
======================================================================

Criar:

WBP_CharacterSelectionScreen

Suggested hierarchy:

CanvasPanel
 ├── BackgroundLayer
 ├── WorldVFXOverlay
 ├── AtmosphericTextLayer
 ├── LogoLayer
 └── CharacterSelectionPanel
      ├── Shadow
      ├── OuterFrame
      ├── Glass
      ├── EnergyFrame
      └── Content
           ├── Header
           │    ├── Title
           │    └── Subtitle
           ├── AccountInfo
           ├── CharacterGrid
           │    ├── CharacterCard
           │    ├── CharacterCard
           │    ├── CharacterCard
           │    └── EmptyCharacterSlot
           ├── Divider
           ├── ActionButtons
           └── Footer

Reusable widgets:

WBP_CharacterCard
WBP_EmptyCharacterSlot
WBP_ActionButton
WBP_PrimaryActionButton
WBP_CharacterSelectionHeader
WBP_DeleteConfirmation

======================================================================
37. BLENDER ASSET PIPELINE
======================================================================

Se não existirem assets adequados, utilize Blender MCP.

Criar modularmente:

SM_FloatingObelisk_A
SM_FloatingObelisk_B
SM_FloatingObelisk_C

SM_CityTower_A
SM_CityTower_B
SM_CityTower_C
SM_CityTower_D

SM_CitySpire
SM_CityRing

SM_ForegroundPlatform
SM_ForegroundRailing
SM_ForegroundAntenna
SM_ForegroundPipe

SM_Bridge_A
SM_Bridge_B

SM_NanoDrone_A
SM_NanoDrone_B
SM_NanoDrone_C

Usar:

hard-surface modeling;
bevel;
weighted normals;
modular design;
efficient topology.

Aplicar UVs corretamente.

Export scale compatível com Unreal.

======================================================================
38. CHARACTER PIPELINE
======================================================================

Se KAEL-7, LYRA e BRONT ainda não existirem como assets completos:

NÃO pare.

Criar placeholders visualmente coerentes.

Prioridade:

1. silhueta correta;
2. cor correta;
3. pose correta;
4. iluminação correta;
5. detalhes.

Ideal:

Skeletal Mesh.

Adicionar:

idle animation;
subtle servo movement;
head movement;
small cloth movement;
energy pulse.

KAEL-7:
subtle scanning.

LYRA:
subtle breathing/hair motion.

BRONT:
slow heavy mechanical idle.

======================================================================
39. COMFYUI
======================================================================

ComfyUI é ferramenta de SUPORTE.

Pode produzir:

- character concepts;
- portrait references;
- card backgrounds;
- storm concepts;
- nano-cloud masks;
- environmental mood;
- city concept passes;
- roughness masks;
- emissive masks;
- decals;
- holographic patterns;
- UI surface textures;
- alternative visual passes.

NÃO gerar toda a interface como um único bitmap.

NÃO substituir componentes UMG por screenshots.

======================================================================
40. MATERIALS
======================================================================

Criar materiais necessários:

M_UI_CyberFrame
M_UI_EnergyBorder
M_UI_EnergySweep
M_UI_Glass
M_UI_Scanline
M_UI_HolographicNoise
M_UI_CardBackground

Material Instances:

MI_UI_Cyan
MI_UI_Magenta
MI_UI_Orange

Environment:

M_CityMetal
M_CityDarkMetal
M_CityEmissive
M_NanoDrone
M_StormVolume
M_CloudVolume

======================================================================
41. FRAME MATERIAL
======================================================================

Dark Metal:

Base:
#081A3C
#050B14
#2B2F3A

Metallic:
0.75–1

Roughness:
0.18–0.35

Subtle brushed metal normal.

Energy trims:

cyan emissive.

Occasional magenta micro details.

======================================================================
42. VOLUMETRIC ENVIRONMENT
======================================================================

Usar:

Volumetric Fog
Volumetric Clouds
Niagara
Lumen
Emissive Materials

A neblina não deve esconder completamente a cidade.

Ela deve revelar camadas.

Objetivo:

foreground
midground
background
far background

claramente distinguíveis.

======================================================================
43. LUMEN
======================================================================

Utilizar Lumen quando adequado ao projeto.

Emissive city surfaces devem contribuir visualmente para o ambiente.

Controlar exposição.

Evitar que:

cyan;
magenta;
lightning

estourem para branco constantemente.

======================================================================
44. UI ENERGY EFFECTS
======================================================================

Criar:

subtle scanlines;
edge glow;
moving energy border;
energy sweep;
small holographic noise;
rare particles.

Evitar visual excessivamente ocupado.

A legibilidade da UI tem prioridade.

======================================================================
45. NIAGARA NANO SWARM
======================================================================

Criar sistema:

NS_NanoSwarm

Behavior:

flocking;
flow field;
spline paths;
vortex zones;
random turbulence.

Particles podem alternar entre:

tiny dark drones;
black shards;
micro silhouettes;
magenta dots.

Clusters devem criar grandes formas orgânicas.

======================================================================
46. NIAGARA LIGHTNING
======================================================================

Criar:

NS_StormLightning

Características:

random strike interval;
branching;
magenta;
purple;
white core;
brief light flash.

Lightning should illuminate nearby volumetric fog.

======================================================================
47. PARALLAX
======================================================================

Adicionar parallax extremamente sutil.

Layers:

Sky
Far Clouds
Far City
Main City
Storm
Floating Structures
Nano Swarms
Foreground
UI

Mouse/gamepad pode deslocar perspectiva levemente.

Máximo visual:

aproximadamente 1–3 graus.

A UI central NÃO deve se deslocar com o mundo.

======================================================================
48. TYPOGRAPHY
======================================================================

LOGO:

custom metallic sci-fi display style.

UI title:

square;
technological;
highly readable.

Pode usar como referência:

Orbitron
Exo 2
Rajdhani
Inter

Não depender dessas fontes se não estiverem disponíveis.

Criar fonte equivalente permitida.

Character names:

bold.

Classes:

high tracking;
color coded.

Secondary text:

clean sans-serif.

======================================================================
49. SOCIAL / DESIGN LANGUAGE
======================================================================

Toda UI deve compartilhar:

same bevel language;
same corner cuts;
same border thickness;
same spacing logic;
same icon weight;
same glow behavior.

Nenhum elemento deve parecer vir de outro UI kit.

======================================================================
50. RESPONSIVIDADE
======================================================================

Testar:

1920×1080
2560×1440
3440×1440
3840×2160

Em widescreen:

NÃO esticar o painel central.

Expandir principalmente o background.

Preservar aspect ratio e proporção visual do painel.

Utilizar:

Anchors
Safe Zones
ScaleBox quando necessário
SizeBox
HorizontalBox
VerticalBox
Overlay

Evitar coordenadas absolutas sempre que possível.

======================================================================
51. INPUT SUPPORT
======================================================================

Suportar:

Mouse
Keyboard
Gamepad

Gamepad:

LEFT / RIGHT:
change card.

A / ENTER:
select / confirm.

B / ESC:
back.

Focus deve ser visualmente claro.

======================================================================
52. CHARACTER SELECTION LOGIC
======================================================================

Default:

KAEL-7 selected.

Ao selecionar novo personagem:

previous selected → normal.

new card → selected.

Atualizar:

SelectedCharacterId
SelectedCharacterData

Atualizar:

glow;
border;
button availability.

======================================================================
53. ENTER WORLD
======================================================================

Ao clicar:

ENTRAR NO MUNDO

executar:

OnEnterWorld(SelectedCharacterId)

Adicionar loading state.

Não hardcode level travel diretamente no widget caso exista arquitetura de GameInstance/SessionManager.

======================================================================
54. CREATE NEW
======================================================================

Quarto slot e botão CRIAR NOVO devem chamar:

OnCreateCharacterRequested

Se character creation ainda não existir:

preparar evento/interface.

Não inventar fluxo incompatível com restante do projeto.

======================================================================
55. DELETE
======================================================================

Button:

APAGAR

somente ativo quando um personagem válido está selecionado.

Abrir modal.

Exemplo:

Deseja realmente apagar KAEL-7?

Exigir confirmação explícita.

======================================================================
56. UI STATES
======================================================================

Preparar:

Idle
LoadingCharacters
Ready
Selecting
EnteringWorld
CreateCharacter
DeleteConfirmation
Error

======================================================================
57. STYLE DATA
======================================================================

Centralizar parâmetros de visual.

Exemplo:

UI_PrimaryCyan
UI_Magenta
UI_Orange
UI_DarkBlue
UI_Black
UI_TextPrimary
UI_TextSecondary
UI_Error
UI_GlowIntensity
UI_BorderThickness
UI_CornerSize
UI_CardGap
UI_AnimationSpeed

Evitar hardcode de cores espalhado em dezenas de widgets.

======================================================================
58. ASSET ORGANIZATION
======================================================================

Organizar aproximadamente:

/Game/UI/CharacterSelection/

Widgets/
Materials/
MaterialInstances/
Textures/
Icons/
Fonts/
Animations/
Data/

/Game/Characters/

Kael7/
Lyra/
Bront/

/Game/Environment/CharacterSelection/

City/
Platforms/
FloatingStructures/
NanoDrones/
Materials/
VFX/
Lighting/

======================================================================
59. PERFORMANCE
======================================================================

Não utilizar Blueprint Tick sem necessidade.

Utilizar:

timers;
animations;
material time;
Niagara internal simulation.

Criar Niagara LOD.

Cidade distante:

Nanite/HLOD quando apropriado.

Evitar centenas de objetos únicos desnecessários.

Usar módulos repetíveis.

UI:

Invalidation quando útil.

RetainerBox apenas se existir benefício real.

======================================================================
60. SCENE DEPTH
======================================================================

O cenário precisa ter enorme profundidade.

Não fazer um skybox plano.

Separar visualmente:

VERY FAR:
sky + storm.

FAR:
city silhouettes.

MID:
main floating city.

NEAR:
obelisks/nanos.

FOREGROUND:
platform.

UI:
screen-space.

======================================================================
61. ART DIRECTION PHRASES
======================================================================

As seguintes frases pertencem à DIREÇÃO ARTÍSTICA e não precisam obrigatoriamente aparecer todas na tela:

HERÓIS NÃO NASCEM. SÃO FORJADOS.

BELEZA NO CAOS.

UM MUNDO MAIOR TE ESPERA.

DESIGN ORIGINAL. HERÓIS ÚNICOS.
MESMA ESSÊNCIA: RESISTIR.

MESMOS SONHOS. NOVAS REALIDADES.

Utilize-as como orientação narrativa.

Não adicionar todas indiscriminadamente ao HUD.

======================================================================
62. CANONICAL SCREEN TEXT
======================================================================

Estes textos SIM pertencem à tela:

AETHER FORGE X

PROTOCOLO ZERO

SELEÇÃO DE PERSONAGEM

MESMOS SONHOS. NOVAS REALIDADES.

Personagens da conta

3 / 4 espaços utilizados

KAEL-7
RANGER
NÍVEL 42
SELECIONADO

LYRA
ASSASSINA
NÍVEL 37

BRONT
DEFENSOR
NÍVEL 28

CRIAR NOVO
PERSONAGEM

FORJE SUA
PRÓPRIA LENDA

ENTRAR NO MUNDO

CRIAR NOVO

APAGAR

VOLTAR

CONEXÃO COM UM MUNDO MAIOR

======================================================================
63. NÃO FAZER
======================================================================

NÃO:

- redesenhar layout;
- transformar cards em cards arredondados;
- aumentar gaps;
- usar estética de website;
- substituir mundo 3D por imagem final única;
- transformar UI em bitmap;
- usar personagens genéricos sem silhueta diferenciada;
- eliminar tempestade;
- eliminar nanos;
- eliminar foreground;
- eliminar obeliscos;
- eliminar iluminação magenta;
- exagerar bloom;
- esconder cidade com fog excessivo;
- misturar cores sem função;
- finalizar com assets quebrados;
- declarar conclusão sem rodar a tela.

======================================================================
64. PIPELINE OBRIGATÓRIO
======================================================================

ETAPA 1
Analisar todas as referências.

ETAPA 2
Inspecionar o projeto Unreal atual.

ETAPA 3
Localizar assets existentes que possam ser reutilizados.

ETAPA 4
Criar data model.

ETAPA 5
Criar assets faltantes em Blender.

ETAPA 6
Criar concept support/textures em ComfyUI.

ETAPA 7
Construir cenário.

ETAPA 8
Criar iluminação.

ETAPA 9
Criar Niagara.

ETAPA 10
Criar Materials.

ETAPA 11
Criar UMG.

ETAPA 12
Criar character cards.

ETAPA 13
Criar estados.

ETAPA 14
Criar interactions.

ETAPA 15
Integrar data/account/save.

ETAPA 16
Rodar.

ETAPA 17
Capturar screenshot.

ETAPA 18
Comparar com referência.

ETAPA 19
Corrigir.

ETAPA 20
Repetir até atingir alta fidelidade.

======================================================================
65. LOOP VISUAL OBRIGATÓRIO
======================================================================

IMPLEMENT
↓
PLAY / PREVIEW
↓
SCREENSHOT
↓
COMPARE
↓
MEASURE DIFFERENCES
↓
FIX
↓
PLAY AGAIN

Não considerar a tarefa concluída depois de apenas criar os widgets.

======================================================================
66. COMPARAÇÃO VISUAL
======================================================================

Comparar explicitamente:

logo position;
logo scale;

panel width;
panel height;

header position;

character grid position;

card proportions;

character silhouette;

character scale;

gaps;

button sizes;

bottom footer;

cyan intensity;

magenta intensity;

orange intensity;

storm density;

lightning density;

floating structures;

city skyline;

foreground platform;

nano swarm placement;

overall contrast;

fog density.

======================================================================
67. CRITÉRIOS DE ACEITAÇÃO
======================================================================

A implementação somente será considerada concluída quando:

1. A composição geral corresponder claramente à arte final.

2. AETHER FORGE X estiver corretamente posicionado.

3. PROTOCOLO ZERO estiver presente.

4. O painel possuir aparência hard-surface sci-fi.

5. SELEÇÃO DE PERSONAGEM estiver corretamente hierarquizado.

6. MESMOS SONHOS. NOVAS REALIDADES. estiver presente.

7. Personagens da conta estiver presente.

8. 3 / 4 espaços utilizados estiver presente.

9. KAEL-7 possuir visual cyan/black correto.

10. KAEL-7 possuir scarf/cape.

11. KAEL-7 possuir rifle.

12. KAEL-7 estiver selecionado por padrão.

13. O estado selecionado possuir strong cyan frame + status bar.

14. LYRA possuir silhueta feminina ágil.

15. LYRA possuir cabelo branco/lilás.

16. LYRA possuir energia magenta.

17. BRONT possuir silhueta muito mais pesada.

18. BRONT possuir energia laranja.

19. Os três personagens possuírem silhuetas claramente diferentes.

20. O slot Create New permanecer visualmente vazio.

21. O + octagonal estiver presente.

22. FORJE SUA PRÓPRIA LENDA estiver presente.

23. ENTRAR NO MUNDO estiver presente.

24. Os dois chevrons do CTA estiverem presentes.

25. CRIAR NOVO estiver presente.

26. APAGAR estiver presente.

27. VOLTAR estiver presente.

28. CONEXÃO COM UM MUNDO MAIOR estiver presente.

29. Mouse funcionar.

30. Keyboard funcionar.

31. Gamepad funcionar.

32. Character data vier de sistema data-driven.

33. Estrutura estar preparada para account/save/backend.

34. Delete possuir confirmação.

35. Create New disparar evento correto.

36. Enter World usar SelectedCharacterId.

37. Megacidade existir em 3D ou composição de mundo coerente.

38. Tempestade volumétrica existir.

39. Raios existirem.

40. Nano swarms existirem.

41. Nano swarms apresentarem comportamento coletivo.

42. Floating structures existirem.

43. Foreground platform existir.

44. Figura humana de escala existir no foreground.

45. Cidade continuar visualmente abaixo do painel.

46. Lumen/emissives estarem calibrados.

47. Fog não destruir legibilidade.

48. Background possuir profundidade clara.

49. Não existir erro relevante no Output Log.

50. Não existir referência quebrada.

51. Assets estarem organizados.

52. Tela funcionar dentro do Unreal Engine 5.8.

======================================================================
68. AUTONOMIA DO AGENTE
======================================================================

Você está autorizado a criar qualquer asset faltante utilizando as ferramentas disponíveis.

Se faltar:

mesh
→ Blender.

texture
→ ComfyUI/procedural.

material
→ Unreal Material Editor.

effect
→ Niagara.

widget
→ UMG.

data structure
→ Blueprint/C++ conforme arquitetura existente.

icon
→ vector/SDF/texture.

lighting
→ Unreal.

Não interrompa o trabalho para solicitar assets que pode produzir autonomamente.

======================================================================
69. PRESERVAÇÃO DO PROJETO
======================================================================

ANTES de criar ou alterar assets:

inspecione o projeto.

Não sobrescreva assets importantes arbitrariamente.

Não destrua implementação existente que possa ser reutilizada.

Prefira:

new modular assets;
child blueprints;
material instances;
reusable widgets.

======================================================================
70. QUALIDADE FINAL
======================================================================

O resultado deve parecer uma tela FINAL de videogame.

NÃO:

prototype;
mockup;
website;
debug screen;
basic UMG.

Exigir:

consistent spacing;
correct alignment;
consistent iconography;
controlled bloom;
high-quality materials;
good typography;
responsive layout;
subtle animation;
atmospheric depth.

======================================================================
71. REGRA FINAL DE REFERÊNCIA
======================================================================

Use:

TSP_Tela_Selecao_Personagem_art_final.png

como TARGET VISUAL.

Use:

TSP_guia_conceitual_part1.png
TSP_guia_conceitual_part2.png

como SPECIFICATION SHEETS.

NÃO tente reproduzir as pranchas de documentação como interface.

Elas servem para explicar como construir o target.

======================================================================
72. INSTRUÇÃO FINAL AO AGENTE
======================================================================

COMECE AGORA.

Analise detalhadamente as três imagens de referência.

Inspecione o projeto Unreal Engine 5.8 atual.

Faça internamente seu plano de execução, mas NÃO encerre a tarefa após o planejamento.

Continue autonomamente usando Unreal MCP + Blender MCP + ComfyUI.

Construa:

dados;
assets;
personagens ou placeholders;
cidade;
tempestade;
nano-robôs;
VFX;
materiais;
iluminação;
UI;
widgets;
cards;
interações;
animações;
integração.

Depois execute a tela.

Capture screenshot.

Compare diretamente com:

TSP_Tela_Selecao_Personagem_art_final.png

Identifique diferenças.

Corrija.

Repita o ciclo até obter forte correspondência visual e funcional.

A tarefa somente termina quando a tela estiver funcionando dentro do Unreal Engine 5.8, visualmente próxima da referência, organizada tecnicamente e sem erros relevantes.

======================================================================
73. REGRA CRÍTICA — PERSONAGENS DA CONTA DO JOGADOR
======================================================================

ESTA SEÇÃO SOBRESCREVE QUALQUER INSTRUÇÃO ANTERIOR QUE POSSA DAR A
ENTENDER QUE KAEL-7, LYRA, BRONT OU QUALQUER OUTRO PERSONAGEM DEVEM
APARECER FIXAMENTE NA TELA.

A TELA DE SELEÇÃO DE PERSONAGEM É 100% DATA-DRIVEN.

NENHUM PERSONAGEM DEVE SER CRIADO OU EXIBIDO AUTOMATICAMENTE
SIMPLESMENTE PORQUE ELE APARECE NA CONCEPT ART.

KAEL-7, LYRA E BRONT EXISTEM NA REFERÊNCIA APENAS COMO EXEMPLOS VISUAIS
DE COMO UM PERSONAGEM DA CONTA DEVE SER APRESENTADO.

A REGRA REAL É:

ACCOUNT DATA
      ↓
LOAD CHARACTERS OWNED BY PLAYER
      ↓
CREATE ONE CARD FOR EACH EXISTING CHARACTER
      ↓
ADD "CRIAR NOVO PERSONAGEM" IF ACCOUNT HAS FREE CHARACTER SLOTS

======================================================================
74. FONTE DOS PERSONAGENS
======================================================================

Os personagens mostrados na tela DEVEM vir exclusivamente dos dados da
conta/save/backend do jogador.

Exemplos possíveis de fonte:

Account Service
Backend API
Database
Save Game
Game Instance
Character Service
Session/Profile Service

O Widget NÃO possui autoridade para inventar personagens.

O Widget recebe uma lista:

PlayerCharacters[]

e constrói a interface com base exclusivamente nela.

Pseudo fluxo:

OnCharacterSelectionOpened()

    AccountData = GetCurrentPlayerAccount()

    Characters = AccountData.Characters

    MaxSlots = AccountData.MaxCharacterSlots

    PopulateCharacterSelection(Characters, MaxSlots)

======================================================================
75. ZERO PERSONAGENS
======================================================================

Se:

Characters.Num() == 0

NÃO mostrar:

KAEL-7
LYRA
BRONT
personagem bloqueado
silhueta de personagem
cards vazios extras
placeholders de personagem

Mostrar SOMENTE:

[ CRIAR NOVO PERSONAGEM ]

A tela deve comunicar claramente que a conta ainda não possui
personagens.

Exemplo:

Personagens da conta                    0 / 4 espaços utilizados

               ┌────────────────────┐
               │                    │
               │         +          │
               │                    │
               │    CRIAR NOVO      │
               │    PERSONAGEM      │
               │                    │
               │ FORJE SUA PRÓPRIA  │
               │      LENDA         │
               │                    │
               └────────────────────┘

Nenhum personagem fictício deve ser mostrado.

======================================================================
76. UM PERSONAGEM
======================================================================

Se a conta possuir apenas um personagem:

Characters.Num() == 1

mostrar:

[ PERSONAGEM REAL DA CONTA ]
[ CRIAR NOVO PERSONAGEM ]

Exemplo:

Personagens da conta                    1 / 4 espaços utilizados

[ PlayerCharacter01 ] [ Criar Novo ]

O personagem existente pode ocupar o primeiro slot.

Não preencher os outros slots com personagens fictícios.

======================================================================
77. DOIS PERSONAGENS
======================================================================

Se:

Characters.Num() == 2

mostrar:

[ Character 01 ]
[ Character 02 ]
[ Criar Novo ]

Header:

2 / 4 espaços utilizados

======================================================================
78. TRÊS PERSONAGENS
======================================================================

Se:

Characters.Num() == 3

mostrar:

[ Character 01 ]
[ Character 02 ]
[ Character 03 ]
[ Criar Novo ]

Header:

3 / 4 espaços utilizados

A aparência desta situação corresponde aproximadamente à concept art,
mas os três personagens devem ser os personagens REALMENTE pertencentes
à conta.

Se os personagens da conta forem:

Ragnar
Zero-17
Astra

a UI deverá mostrar:

Ragnar
Zero-17
Astra
Criar Novo

e NÃO:

KAEL-7
LYRA
BRONT.

======================================================================
79. QUATRO PERSONAGENS / CONTA CHEIA
======================================================================

Se:

Characters.Num() >= MaxCharacterSlots

e por exemplo:

MaxCharacterSlots == 4

mostrar:

[ Character 01 ]
[ Character 02 ]
[ Character 03 ]
[ Character 04 ]

Header:

4 / 4 espaços utilizados

NÃO mostrar o card:

CRIAR NOVO PERSONAGEM

Também:

CreateNewButton deverá ficar escondido ou desabilitado conforme o
design do sistema.

Preferência:

esconder o botão "CRIAR NOVO" quando não existir espaço disponível.

Caso seja necessário mantê-lo por consistência visual:

Disabled = true

e fornecer feedback:

"Todos os espaços de personagem estão sendo utilizados."

======================================================================
80. MAX CHARACTER SLOTS NÃO DEVE SER HARDCODED
======================================================================

Não assumir permanentemente:

MaxCharacterSlots = 4

Embora a concept art mostre:

3 / 4 espaços utilizados

o valor máximo deve vir dos dados da conta.

Exemplo:

AccountProfile.MaxCharacterSlots

Assim o sistema poderá futuramente suportar:

2 slots
4 slots
5 slots
8 slots
premium slots
temporary slots
additional purchased/unlocked slots

A interface deverá se adaptar.

======================================================================
81. CHARACTER GRID DINÂMICO
======================================================================

Não criar quatro CharacterCards fixos dentro do Designer.

ERRADO:

CharacterCard_Kael
CharacterCard_Lyra
CharacterCard_Bront
CharacterCard_New

CORRETO:

CharacterContainer

e durante runtime:

ClearChildren()

for CharacterData in PlayerCharacters:

    CharacterCard = CreateWidget(WBP_CharacterCard)

    CharacterCard.SetCharacterData(CharacterData)

    CharacterContainer.AddChild(CharacterCard)

Depois:

if PlayerCharacters.Num() < MaxCharacterSlots:

    CreateCard = CreateWidget(WBP_EmptyCharacterSlot)

    CharacterContainer.AddChild(CreateCard)

Portanto a quantidade de cards deve ser criada dinamicamente.

======================================================================
82. KAEL-7 / LYRA / BRONT SÃO REFERÊNCIAS DE ESTILO
======================================================================

Os seguintes personagens da concept art:

KAEL-7
LYRA
BRONT

devem ser interpretados SOMENTE como STYLE REFERENCES.

Eles demonstram:

como enquadrar personagem;
como iluminar personagem;
como mostrar nome;
como mostrar classe;
como mostrar nível;
como colorir classe;
como mostrar estado selecionado;
como diferenciar silhuetas.

Eles NÃO representam personagens obrigatórios da conta.

Caso o backend realmente retorne:

KAEL-7

então KAEL-7 pode aparecer.

Caso contrário:

NÃO criar KAEL-7.

A mesma regra vale para:

LYRA
BRONT
qualquer outro personagem demonstrativo.

======================================================================
83. DADOS NECESSÁRIOS PARA CADA PERSONAGEM
======================================================================

Cada card deve ser construído a partir de:

FCharacterSelectionData

mínimo:

CharacterId
AccountId
CharacterName
CharacterClass
CharacterLevel

Portrait

ClassIcon

PrimaryColor
SecondaryColor

SkeletalMesh
CharacterPreviewActor

IsPlayable
IsLocked

LastPlayed

SaveSlot

WorldId

Optional:

CharacterAppearanceData
EquipmentData
CustomizationData

O Widget deve consumir esses dados.

Não deve conhecer previamente nomes de personagens.

======================================================================
84. PERSONAGEM SELECIONADO DEFAULT
======================================================================

REMOVER a regra anterior:

"KAEL-7 selected by default."

Nova regra:

Se existir:

LastPlayedCharacterId

e este personagem ainda existir na conta:

SelectedCharacter =
    LastPlayedCharacterId

Caso contrário:

if Characters.Num() > 0:

    SelectedCharacter = Characters[0]

Caso:

Characters.Num() == 0:

    SelectedCharacter = NONE

e automaticamente dar foco ao:

CRIAR NOVO PERSONAGEM.

======================================================================
85. ZERO CHARACTERS STATE
======================================================================

Quando:

Characters.Num() == 0

estado da tela:

EmptyAccount

Comportamento:

ENTER WORLD:
hidden ou disabled

DELETE:
hidden ou disabled

CREATE NEW:
primary action

O card CRIAR NOVO deve ganhar maior destaque visual.

Gamepad focus inicial:

Create New Character.

======================================================================
86. CHARACTER EXISTS STATE
======================================================================

Quando existir pelo menos um personagem:

SelectedCharacter != NONE

habilitar:

ENTRAR NO MUNDO

APAGAR

e preencher os dados do card.

Create New somente aparece quando:

Characters.Num() < MaxCharacterSlots

======================================================================
87. PERSONAGEM DELETADO
======================================================================

Após confirmação de delete:

DeleteCharacter(CharacterId)

Após sucesso:

RefreshAccountCharacters()

NÃO remover apenas o card visualmente e assumir que os dados foram
deletados.

Fluxo:

Request Delete
    ↓
Backend / Save System
    ↓
Delete Success
    ↓
Reload Character List
    ↓
Rebuild UI

Após reconstrução:

se ainda houver personagens:

selecionar personagem válido.

se não houver:

entrar em EmptyAccount.

======================================================================
88. PERSONAGEM CRIADO
======================================================================

Depois da criação:

CharacterCreation
      ↓
Character Created Successfully
      ↓
Save / Backend
      ↓
Refresh Character List
      ↓
Return to Character Selection
      ↓
New Character appears automatically

Preferencialmente selecionar automaticamente o novo personagem.

======================================================================
89. FLUXO REAL DA TELA
======================================================================

LOGIN

↓

LOAD ACCOUNT

↓

FETCH CHARACTERS

↓

IF NO CHARACTERS:

    SHOW:
    + CRIAR NOVO PERSONAGEM

    HIDE/DISABLE:
    ENTRAR NO MUNDO
    APAGAR

ELSE:

    SHOW:
    ALL CHARACTERS FROM ACCOUNT

    IF FREE SLOT:
        SHOW CREATE NEW

    SELECT:
        LastPlayedCharacter
        OR
        First Character

↓

PLAYER SELECTS CHARACTER

↓

ENTRAR NO MUNDO

======================================================================
90. EXEMPLOS OBRIGATÓRIOS DE COMPORTAMENTO
======================================================================

EXEMPLO A

Conta:

Characters = []

MaxSlots = 4

Resultado:

0 / 4 espaços utilizados

[ CRIAR NOVO PERSONAGEM ]


EXEMPLO B

Conta:

Characters =
[
    "ZERO-21"
]

Resultado:

1 / 4 espaços utilizados

[ ZERO-21 ]
[ CRIAR NOVO PERSONAGEM ]


EXEMPLO C

Conta:

Characters =
[
    "ALPHA"
    "MIRA"
]

Resultado:

2 / 4 espaços utilizados

[ ALPHA ]
[ MIRA ]
[ CRIAR NOVO PERSONAGEM ]


EXEMPLO D

Conta:

Characters =
[
    "KAEL-7"
    "LYRA"
    "BRONT"
]

Resultado:

3 / 4 espaços utilizados

[ KAEL-7 ]
[ LYRA ]
[ BRONT ]
[ CRIAR NOVO PERSONAGEM ]


EXEMPLO E

Conta:

Characters =
[
    "ALPHA"
    "MIRA"
    "BRONT"
    "VOLT"
]

Resultado:

4 / 4 espaços utilizados

[ ALPHA ]
[ MIRA ]
[ BRONT ]
[ VOLT ]

Não mostrar Create New.

======================================================================
91. CRITÉRIO DE ACEITAÇÃO ADICIONAL
======================================================================

A implementação será considerada INCORRETA se:

- KAEL-7 aparecer sem existir na conta;
- LYRA aparecer sem existir na conta;
- BRONT aparecer sem existir na conta;
- qualquer personagem demonstrativo aparecer automaticamente;
- forem criados personagens apenas para preencher visualmente slots;
- slots inexistentes mostrarem personagens fake;
- o número "3 / 4" estiver hardcoded;
- a quantidade máxima de slots estiver hardcoded exclusivamente na UI;
- o card Create New continuar aparecendo quando não houver espaço;
- Enter World estiver habilitado sem personagem;
- Delete estiver habilitado sem personagem.

A implementação correta deve sempre obedecer:

VISIBLE CHARACTERS = CHARACTERS OWNED BY CURRENT ACCOUNT

e:

CREATE NEW VISIBLE =
CURRENT_CHARACTER_COUNT < MAX_CHARACTER_SLOTS

======================================================================
92. REGRA FINAL SOBRE PERSONAGENS
======================================================================

A CONCEPT ART DEFINE A APARÊNCIA.

A CONTA DO JOGADOR DEFINE O CONTEÚDO.

Nunca confundir essas responsabilidades.

CONCEPT ART:
como mostrar.

ACCOUNT/BACKEND:
o que mostrar.

Portanto:

NÃO EXISTE PERSONAGEM NA CONTA
→ NÃO EXISTE CARD DO PERSONAGEM NA TELA.

EXISTE PERSONAGEM NA CONTA
→ CRIAR CARD COM OS DADOS DESSE PERSONAGEM.

NÃO EXISTEM PERSONAGENS
→ MOSTRAR SOMENTE CRIAR NOVO PERSONAGEM.

EXISTEM VAGAS
→ MOSTRAR CRIAR NOVO PERSONAGEM DEPOIS DOS PERSONAGENS EXISTENTES.

NÃO EXISTEM VAGAS
→ NÃO MOSTRAR CRIAR NOVO PERSONAGEM.

ESTA REGRA TEM PRIORIDADE SOBRE QUALQUER EXEMPLO VISUAL OU PERSONAGEM
MOSTRADO NAS IMAGENS DE REFERÊNCIA.
