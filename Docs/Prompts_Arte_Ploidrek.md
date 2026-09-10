# Pacote de prompts — arte do PloidrekRPG

> **Para que serve:** é a ponte entre o **conceito** (a pasta `Art/`, a paleta `Docs/SteampunkPalette.md` e as
> descrições em `Data/DT_Classes.csv` e `Data/DT_Chassis.csv`) e as **ferramentas generativas** (Gemini,
> ChatGPT/OpenAI, ComfyUI). Os prompts estão em **inglês** de propósito: modelo de imagem responde melhor; o
> conceito vem do português do projeto.

## 1. O ciclo de produção

```
conceito (Art/ + paleta + DataTable)
   └── prompt deste documento  ──►  ferramenta generativa (imagem / 3D / textura)
         └── arquivo em Art/generated/<familia>/
               └── eu OLHO o arquivo (PNG aberto e conferido, não só o nome)
                     └── aprovado? importa no engine (Python headless) e pluga:
                           DataTable (retrato do chassi/classe) · Material · UMG · Mesh
                     └── reprovado? volta para o prompt com o motivo escrito
```

**Regra de ouro:** nada entra na build sem passar pela checklist do §7 — e nada é aceito só porque
"parece bonito": precisa casar com a paleta, com o estilo das amostras e com o conceito do DataTable.

## 2. Bloco de estilo (colar no começo de TODO prompt)

```text
STYLE ANCHOR — industrial-forge steampunk crossed with action sci-fi. Consistent AAA game art,
physically based rendering, crisp hard-surface detail, high contrast, restrained palette.
Materials: polished brass #c9a227, brushed brass #a8842c, copper #b87333, oxidized copper #8a4a2a,
forged iron #3a3a42, dark steel #2a2a30, rivets #d8c890, amber glass #ffb040, steam #c8c0a8.
Environment light: dusk sky (#1a2a5e to #3a4a80), forge-horizon glow (#c86a30 / #ff9040), warm amber
key light, cold cyan rim light (#40e0ff), low exposure, deep shadows.
No text, no watermark, no signature, no UI overlay, no photographic lens artifacts.
```

**Referências para anexar** (a ferramenta deve usar como base de estilo, não como conteúdo):

| Arquivo | Serve de base para |
|---|---|
| `Art/generated/steampunk/brass_wall.png` | textura de metal com rebites → materiais, UI, cenário |
| `Art/Classes/sample_tank.jpg` | estilo de **personagem** (cel shading, contorno grosso, robô azul/branco) |
| `Art/generated/world/city_skyline.png` | escala e clima da megacidade |
| `Art/generated/forest/*.png` | bioma biomecânico (orgânico + metal) |
| `Art/reference/` (FBX de Megaman, Tank_Hi3D, etc.) | proporção e articulação de referência |

## 3. Cobertura: o que existe e o que falta

| Família | Existe | Falta |
|---|---|---|
| Texturas de ambiente (steampunk) | 6 imagens base (albedo visual) | **pacote PBR** de cada uma (normal, roughness, metallic, AO) e versão **tileable** |
| Personagens de classe | 7 ilustrações em `Art/Classes/` | confirmação do mapeamento (ver §8) e **fundo transparente** |
| Chassis jogáveis | 0 retratos (só o manequim da engine) | **10 retratos** + turnaround 3D dos 10 |
| Modelos 3D de Runner | FBX/GLB soltos (cleric, Blubot, floresta) | **malha + rig** de cada chassi, no mesmo esqueleto |
| UI | `brass_wall` (vira moldura) | moldura 9-slice, ornamento de canto, ícones de habilidade, fundo de tela |
| Inimigos | `mech_beetle`, `mech_rabbit`, `mech_scorpion` | os Lordes DataCores (ver `Docs/Bestiario_Lordes_DataCores.md`) |

## 4. Prompts — CHASSIS (retrato + turnaround 3D)

Um retrato por chassi, com o conceito do DataTable dentro do prompt. **Troque o campo `CONCEPT`** pelo texto
da tabela abaixo e mantenha o resto igual: é o que garante que os dez pareçam do mesmo jogo.

```text
[STYLE ANCHOR]

Full-body character sheet of a sci-fi humanoid combat chassis, CONCEPT.
Two deliverables in one sheet, left to right: (1) T-pose turnaround with three orthographic views —
front, side, back — neutral pose, arms straight out, palms down; (2) one 3/4 hero pose.
Anatomy: seven and a half heads tall, athletic, believable joints, visible mechanical articulation at
shoulders, elbows, hips and knees, segmented armor plates with panel lines and rivets, no exposed flesh,
radiator fins and cabling where the concept asks for it.
Rendering: cel-shaded with thick dark outlines and flat color blocks with one shadow tone (matching the
attached character reference), armor in white and mid blue with brass and copper accents, amber emissive
core detail, cyan rim light. Flat plain mid-gray background, even studio light, no cast shadow on the floor,
no perspective distortion. 4096x4096, sharp edges, no text.
```

| Chassi | CONCEPT (colar no prompt) |
|---|---|
| **Charger** | Stocky siege chassis built for impact: thick plates, heavy servos, brute proportions, seismic stomp |
| **Vitaspark** | Slender speed chassis: light hips, dorsal heat dissipators, aerodynamic fins, built to be gone before you look |
| **Aetheric** | Slender chassis bonded to cosmic/quantum energy sources: floating core fragments, ether conduits glowing with soft green-aether light |
| **Techno** | Slender analytical chassis: brain-core head with sensor array, data conduits, holographic analysis plates on the forearms |
| **Droneframe** | Small fragile recon chassis: compact frame, single large optic, antenna cluster, stealth plating, support drone hovering beside it |
| **Forgekin** | Stocky living-forge tank: hydraulic arms, fusion-welded armor, industrial forge vents on the back, calm heavy stance |
| **Ghostnet** | Slender semi-optical infiltration chassis: camouflage panels, refraction plating, no visible face, patience in the silhouette |
| **Vox** | Fragile human-passing chassis with expressive face and vocal resonance emitters in the throat and chest, elegant, influencer-like |
| **Overcore** | Unstable overcharged chassis: cracked core housing, venting steam and sparks, asymmetric plating, barely contained power |
| **Cryonix** | Stocky cryogenic chassis: cryo core, frost on the plating, cooling coils, everything it touches slows down |
| **Clyffen** *(extinto — só para registro)* | Extinct 11th model in direct symbiosis with ether: ethereal translucent plating, ancient elegant design, faded and broken |

### 4b. O método que funcionou (v001 → v002) — e que fica valendo

A **v001** produziu dez chassis com cor e material certos e **a mesma silhueta**: mesma pose, mesmo volume,
câmera de frente. Parecia um set bonito e era um **sistema de design quebrado** — o tipo de erro que passa
despercebido olhando imagem por imagem.

Foi a **folha de contato em silhueta pura** que pegou (`Tools/art/contact_sheet.py`, que joga cada asset para
preto no branco): com dez formas iguais lado a lado, o problema ficou óbvio em dois segundos.

**O que a v002 mudou no prompt** — e que passa a ser obrigatório para chassis e classes:

```text
[STYLE ANCHOR]
Camera: looking DOWN at about 55 degrees above the horizon (top-down action game view): you see the top of
the shoulders, the helmet crest and the whole back unit. Dorsal silhouette is the main read.
BODY TYPE: <o tipo do DataTable — STOCKY / SLENDER / FRAGILE / UNSTABLE, com as consequências de volume>
SHAPE SIGNATURE: <uma ideia de forma única daquele asset, não genérica>
POSE: <pose diferente por asset — quem está agachado não pode ter a mesma pose de quem está plantado>
[regras de leitura + pedido da silhueta preta no canto da imagem]
```

**Truque que vale manter:** pedir *"no canto superior direito, desenhe também a silhueta preta do mesmo
personagem sobre branco"*. O modelo passa a desenhar **pensando em forma** — e a folha de contato ganha o
teste de leitura de graça.

**Resultado medido:** na v001, dez silhuetas pertenciam a **duas famílias** (troncudo e esguio). Na v002, cada
chassi tem forma própria: Vitaspark é uma cunha agachada com aleta dorsal, Ghostnet é uma fera baixa de braços
longos, Droneframe é um corpo pequeno com drone pairando, Forgekin tem chaminés fumegando, Techno é esguio e
ereto com painel holográfico.

**Limite honesto da v002:** o modelo **ignora a instrução de câmera** — saiu em três-quartos de frente (~30°),
não em 55° de cima. Ou seja: são **folhas de design**, ótimas para decidir forma e cor, mas **não** validam
ainda o que o jogador vê de cima. Essa validação é no 3D, com a câmera do jogo apontada para a malha — que é
exatamente o próximo passo da cadeia (imagem → Hunyuan3D → Rigify → UE).

**Os dois mais fracos da v002** (candidatos a v003): **Overcore** (pedi instável e assimétrico; saiu um mech
equilibrado com núcleo aceso) e **Aetheric** (os fragmentos flutuantes ficaram discretos demais).

## 5. Prompts — CLASSES (retrato de interface)

O estilo é o das amostras existentes: **ilustração, contorno grosso, cor chapada**, não render 3D.

```text
[STYLE ANCHOR]

Full-body standing character illustration, three-quarter view, CLASSE_CONCEITO.
Art style: clean cel-shaded game art, thick dark outlines, flat color blocks with a single shadow tone,
hard-surface robot design, confident idle stance, occasional energy glow on the weapon or hands.
ORIGINAL design: use the attached samples only as a level-of-readability reference for silhouette,
proportion and line weight — do not copy their designs and do not imitate any existing game franchise.
Background: fully transparent (deliver PNG with alpha) or flat white for cutting out.
1024x1536 portrait, centered, whole body inside frame, no cropping of feet or hands, no text.
```

| Classe | CLASSE_CONCEITO (do `DT_Classes.csv`) |
|---|---|
| **Blaster** (DPS) | ranged weapon specialist: plasma rifle/cannon, penetrant charged shot, targeting visor |
| **Blade** (DPS) | melee duelist: energy blade(s), light armored frame, counter-attack stance, thin bleeding energy trails |
| **Breaker** (Tank) | front-line tank: tower shield and heavy gauntlets, area taunt emitter, reactive barrier plates |
| **Support** (Support) | healer/tactician: recovery-zone emitter, support drone, soft green-white energy, calm pose |
| **Engineer** (Control) | drone and turret specialist: autonomous drone, deployable turret on the back, tool arm, traps on the belt |
| **Sentinel** (Hybrid) | defensive field manipulator: projected energy shield blocking projectiles, interceptor stance |

## 6. Prompts — TEXTURAS (pacote PBR jogável)

As seis imagens atuais são ótimas como **albedo visual**, mas jogo AAA pede o **conjunto de mapas**. Gere um
conjunto por material, sempre do mesmo recorte:

```text
[STYLE ANCHOR]

Seamless tileable PBR material set, 2048x2048, of DESCRICAO_DO_MATERIAL.
Deliver five separate maps, no baked lighting and no baked shadow in albedo: albedo (base color only),
normal (tangent space, OpenGL), roughness, metallic, ambient occlusion.
Uniform texel density, no visible seam on any edge, no scale reference object, no perspective,
no text. Tileability is critical: the four borders must match when repeated.
```

| Material | DESCRICAO_DO_MATERIAL |
|---|---|
| **Latão com rebites** | worn polished brass plate with a grid of raised rivets on the edges and scratches (base: `brass_wall.png`) |
| **Cobre oxidado** | copper sheet with green-brown oxidised patches, dripping patina, aged industrial |
| **Ferro forjado** | forged iron beam with hammer marks, soot, mill scale |
| **Aço escuro** | dark steel plate, brushed, oil-stained, subtle machining marks |
| **Vidro âmbar** | amber glass panel with metal frame, glow from behind, dust on the surface |
| **Vapor / névoa** | soft volumetric steam texture (alpha), cloudy wisps, seamless, for particles and the menu veil |

## 7. Prompts — INTERFACE

```text
[STYLE ANCHOR]

(A) Ornate steampunk UI frame, 9-slice friendly: decorated corners with rivets, gears and scrollwork,
plain empty stretchable center, thick brass edge with subtle bevel, PNG with transparency, 1024x1024.
(B) Game UI ability icon, single centered symbol only, high contrast, readable at 64 px,
brass-and-glass medallion style, transparent background, 512x512.
(C) Menu backdrop: wide establishment shot of a Victorian megacity under a dusk sky, brass and copper
rooftops, amber windows, steam vents, low angle from a rooftop, cinematic depth, 2560x1440, no text.
```

## 8. Pendências deste pacote (o que eu preciso de você)

1. **Mapeamento das classes x imagens.** Minha leitura das amostras atuais:

| Imagem | Classe | Confiança |
|---|---|---|
| `sample_tank.jpg` | **Breaker** (Tank — defesa e HP) | alta |
| `sample_cleric.jpg` | **Support** (aquele que cura) | alta |
| `sample_defender.jpg` | **Sentinel** (escudo de energia que defende o aliado) | alta |
| `sample_hunter.jpg` | **Engineer** (controle/silence) | média — confirmar |
| `sample_suport.jpg` | **Blaster** ("o que explode") | média — confirmar |
| `sample_maverik.jpg` | **Blade** — é **classe**, não chassi (corrigido pelo autor); garras e porte pesado leem como corpo a corpo crítico | média |

> ⚠️ **`sample_maverik.jpg` está na lista de IP** (`IP-004` do backlog, junto de `Megaman.*` e
> `generated/world/{reploid,maverick}.png`): serve de **conceito de silhueta**, nunca como asset do build.

2. **Chave de API** (em variável de ambiente, nunca no repositório) **ou** ComfyUI rodando em
   `localhost:8188`. Sem um dos dois eu **não gero** imagem nova — o que eu faço sem eles é usar o que
   já existe, escrever prompt (este documento) e preparar/importar/plugar o que chegar.
3. **Rigs:** decidir se os 10 chassis usam **um esqueleto comum** (recomendado: um rig, dez malhas) ou rigs
   próprios. Isso muda o prompt de 3D: com esqueleto comum, o turnaround precisa vir em T-pose com as
   mesmas proporções de ombro/quadril, e a IA não decide isso.

## 9. Que tipo de entrega é cada peça (a pergunta "cobrir superfície ou ser peça?")

Não é um "3D ou 2D" global: é **por entrega**. E é isso que decide o formato do prompt e do arquivo:

| Entrega | Natureza | 3D ou 2D | O que decide |
|---|---|---|---|
| Parede, piso, painel, metal de cenário | **superfície** | **3D PBR obrigatório** (albedo + normal + roughness + metallic + AO) | requisito técnico; o estilo entra só na paleta |
| Personagem, prop, arma | **peça** | **3D de verdade**: malha + UV + LOD (+ rig, se anima) | é o que o jogo renderiza; estilo = shading (PBR × toon) |
| Retrato, ícone, card de UI | **imagem** | **2D** (cel ou render, à escolha) | estilo de interface; **não** alimenta o 3D |
| Conceito que entra no image→3D | **insumo** | **2D, mas em estilo render limpo** | o gerador de malha precisa de forma e sombreamento limpos; contorno grosso e linha dura piora a malha |

**Consequência prática:** a *superfície* e a *peça* são sempre 3D; o 2D aparece em duas funções — a
**representação** (ícone/retrato) e o **insumo** do image→3D. E o insumo deve ser render, não cel,
quando o destino for virar malha.

> **Três identidades competindo hoje no projeto** (a decisão que trava o acabamento AAA):
> (A) steampunk latão/âmbar de `SteampunkPalette.md` — é o que o menu usa;
> (B) sci-fi neon ciano/magenta da key art Aether Forge em `Art/Gemini_Generated_Image_*.jpeg`;
> (C) anime 2D cel das amostras em `Art/Classes/`. Escolher **uma** — ou uma fusão declarada
> (ex.: metal escuro + latão da paleta como matéria e neon ciano como energia) — é pré-requisito de qualquer
> lote de arte.

## 9b. **Cor: o núcleo é do chassi, o corpo é do jogador** (modelo do autor)

**O modelo (autor):** o jogador escolhe as cores do **corpo**, sempre em **três partes** — **70%**, **20%** e
**10%** — e o **núcleo tem cor fixa**, ligada ao **chassi e às habilidades dele**: quem causa frio tem núcleo
azul como frio; o tanque, marrom escuro como pedra; quem explode, outra coisa; e assim por diante.

Isso separa duas coisas que eu tinha juntado na primeira proposta:

| | Quem decide | Papel |
|---|---|---|
| **Cor do núcleo** | **o chassi** (dado fixo) | identidade e leitura em combate: diz **o que aquele chassi faz** |
| **Cores do corpo** (70/20/10) | **o jogador** | expressão: quem é aquele **personagem** |

### O núcleo, por habilidade (a regra do autor aplicada aos 10)

| Chassi | Habilidade (DT_Chassis) | Núcleo | Por quê |
|---|---|---|---|
| **Charger** | Investida Sísmica (derruba e causa dano em área) | **laranja magmático** `#ff6a00` | impacto e calor |
| **Vitaspark** | Crítico Ampliado | **amarelo elétrico** `#ffd91a` | faísca, precisão |
| **Aetheric** | Restauração de Éter | **verde-aether** `#40ffc0` | é o **Aether da paleta**: Éter é verde no projeto |
| **Techno** | Protocolo de Deflexão (defesa reativa por análise) | **azul de dado** `#2673ff` | leitura, cálculo |
| **Droneframe** | Drone de Apoio | **verde-sensor** `#9ad94c` | sensor e apoio |
| **Forgekin** | Armadura Forjada (reduz dano, +HP) | **bronze de forja** `#b35c1e` | o tanque: pedra e metal fundido |
| **Ghostnet** | Camuflagem Ativa | **violeta fantasma** `#994ce6` | o que se esconde |
| **Vox** | Voz Nula (silencia e drena Éter) | **magenta de voz** `#ff4cb2` | som, presença |
| **Overcore** | Superaquecimento (pulso a 25% de vida) | **branco-incandescente** `#fff0c4` | sobrecarregado: o único quase branco, lê por **valor** |
| **Cryonix** | Lentidão Criogênica | **azul de frio** `#5ec8ff` | frio, exatamente como o autor pediu |
| *Clyffen (extinto)* | — | âmbar pálido, morto | memória do 11º modelo |

**Efeito colateral bom:** a colisão que eu tinha encontrado (§ abaixo) some por **semântica**, não por gosto —
Charger (magma), Forgekin (bronze) e Overcore (branco) deixam de ser três vermelhos e passam a ser três ideias
diferentes.

### As três partes do corpo (70/20/10)

| Parte | Onde pega | Papel | Padrão sugerido |
|---|---|---|---|
| **70% — massa** | placas grandes: peito, costas, coxas, braços | é o que faz o personagem ser visto de longe | aço claro / branco neutro |
| **20% — secundária** | ombreiras, canelas, capacete | dá o desenho e separa de longe o aliado do inimigo | latão escovado da paleta |
| **10% — acento** | juntas, filetes, painéis pequenos | detalhe fino; **nunca** é o núcleo | cobre oxidado |

**Regra dura:** o núcleo **não entra** no 10%. Se o núcleo pudesse ser repintado, ele deixaria de informar — e a
única diferença entre herói e Apagado (§9d) é a **luz**, firme num e falhando no outro.

### O que isso exige de dado e código

| Onde | O quê |
|---|---|
| `FRunnerChassisData` | **`CoreColor`** (fixo, identidade) — e a `AccentColor` atual deixa de ser \"a cor do chassi\" e vira **sugestão de paleta de corpo** para o jogador |
| `FRunnerCharacterProfile` | **`ColorPrimary`, `ColorSecondary`, `ColorAccent`** — escolha do jogador, salva com o personagem |
| Material | o overlay `M_RunnerAccent` passa a receber as três cores do corpo; o núcleo usa um **material próprio emissivo** com `CoreColor` |
| UI | seletor de **três cores** na criação (com a prévia ao lado do núcleo fixo), não um picker único |

### Nota de método: o teste que já foi feito

Antes deste modelo, eu testei a ideia do "núcleo colorido" desenhando cada núcleo em 128, 56 e **24 px**
(`Tools/art/core_color_test.py`, o tamanho em que ele aparece com a câmera do jogo) e **as cores antigas
colidiam**: Charger `#d94026`, Overcore `#ff2626` e Forgekin `#d98026` eram o mesmo ponto a 24 px, e quatro
frios ficavam na mesma faixa. A tabela acima resolve isso, e o teste é o que valida: **rodar de novo depois de
aplicar, e conferir que os dez se distinguem a 24 px**.

### Título antigo desta seção (mantido para rastreio)

**Núcleo de identidade** — a ideia do autor, testada antes de virar regra

**A ideia (autor):** o núcleo que eu desenhei em âmbar fixo passa a ser **o indicador do chassi** — cada chassi
com o núcleo na cor dele. Faz sentido: com câmera alta e distante, um **ponto emissivo colorido no peito** é o
elemento mais legível do personagem, mais do que qualquer detalhe de placa. E ele já tem casa nos dados:
`FRunnerChassisData::AccentColor`.

**O teste (`Tools/art/core_color_test.py`):** desenhei o núcleo de cada chassi em 128, 56 e **24 px** (o tamanho
em que ele aparece com a câmera do jogo). Resultado honesto: **as cores atuais não sustentam a ideia**.

| Colisão encontrada | Quem |
|---|---|
| três vermelhos/laranjas no mesmo canto do círculo cromático | **Charger** `#d94026` (matiz 9), **Overcore** `#ff2626` (matiz 0) e **Forgekin** `#d98026` (matiz 30) — a 24 px, Charger e Overcore são o mesmo ponto |
| quatro frios na mesma faixa | **Droneframe** `#33e6b2` (163), **Aetheric** `#33e6ff` (187), **Cryonix** `#99d9ff` (202) e **Techno** `#2673ff` (219) |
| amarelo duplicado | **Vitaspark** `#ffd91a` (50) e **Clyffen** `#ffcc33` (45) — sem problema de jogo, porque o Clyffen é extinto e nunca é escolhido |

**Proposta de correção** (preserva a intenção de cada chassi e abre os matizes, com um outlier de *valor* para
o Overcore, que fica inconfundível por ser o único quase branco):

| Chassi | Hoje | Proposto | Por quê |
|---|---|---|---|
| Charger | `#d94026` | **`#d94026`** (mantém) | vermelho de impacto |
| Forgekin | `#d98026` | **`#ff7a00`** | vira o laranja de forja, longe o bastante do vermelho do Charger |
| Vitaspark | `#ffd91a` | **`#ffd91a`** (mantém) | amarelo elétrico |
| Droneframe | `#33e6b2` | **`#9ad94c`** | verde-espectral de reconhecimento, sai da família do ciano |
| Aetheric | `#33e6ff` | **`#40ffc0`** | é literalmente o **Aether da paleta** (`#40ffc0`) — hoje o chassi do Éter usa ciano, que é cor de gelo |
| Cryonix | `#99d9ff` | **`#5ec8ff`** | ciano de gelo, mais saturado para não confundir com o branco-azulado do Techno |
| Techno | `#2673ff` | **`#2673ff`** (mantém) | azul de dado |
| Ghostnet | `#994ce6` | **`#994ce6`** (mantém) | violeta fantasma |
| Vox | `#ff4cb2` | **`#ff4cb2`** (mantém) | magenta de voz |
| Overcore | `#ff2626` | **`#fff0c4`** | branco-incandescente: sobrecarregado é o único quase branco, então se lê por **valor**, não por matiz |

**Segundo diferenciador (não depende de cor):** a **forma do núcleo pelo tipo de corpo**, que já existe no
DataTable — é o que salva o leitor daltônico e o que dá leitura quando a cor some no fundo:

| Tipo de corpo | Forma do núcleo |
|---|---|
| **Stocky** | hexágono largo, com aro de rebites |
| **Slender** | fenda vertical fina, alta |
| **Fragile** | pequeno disco redondo, com três respiros |
| **Unstable** | rachado e assimétrico, com sangramento de luz nas fissuras |

> **Status:** a forma por tipo de corpo já vale como regra de arte (não muda dado nenhum). A troca de **cor**
> depende do autor, porque mexe em `Data/DT_Chassis.csv` — que também alimenta a tinta do corpo no jogo e os
> ícones. Aplicada a cor, eu reimporto o DataTable e regero a arte dos chassis na v003.

## 9b.1 **O chassi é o cérebro-cristal — o corpo é da CLASSE** (correção do autor)

**O que eu tinha errado:** tratei o chassi como **corpo** (Stocky/Slender/Fragile/Unstable viraram tipo de corpo de
um robô humanoide). O autor corrigiu: **o chassi é igual para todos** — ele é um **pequeno cérebro de cristal com
uma aura**, *"como aquela suntuosa formação que fica ao redor do gelo, um vento visível, gelado, glowing"*.

| | O que é | O que define |
|---|---|---|
| **Chassi** (10) | **cérebro-cristal + aura** — a mesma forma física para todo Runner | o **núcleo**: cor e comportamento da aura (§9b), e a **habilidade** |
| **Classe** (6) | **o corpo** que o Runner usa | a **anatomia** e a **apresentação** (as cinco de §9c) |

**Consequência de leitura do modelo:** o fluxo é **chassi → classe**, e é por isso que ele funciona assim na tela:
primeiro você escolhe **a luz** (o que o seu Runner *faz*), depois escolhe **o corpo** (quem o seu Runner *é*).
Os dois eixos são independentes — 10 × 6 = as **60 combinações** que a suíte de automação já valida em
`Runner.Fluxo.ChassiEClasse`.

### O que isso muda na arte

| Antes (errado) | Agora |
|---|---|
| arte do chassi = robô humanoide de 10 tipos | arte do chassi = **10 cérebros-cristal com 10 auras** (cor e forma do §9b) |
| a `Body` do `DT_Chassis` virou tipo de corpo | a `Body` passa a descrever a **forma da aura**: `Slender` = fitas longas e finas; `Stocky` = casca densa e larga; `Fragile` = véu fino que tremula; `Unstable` = rompida, espetando e falhando |
| a malha do chassi apontava para o manequim | a malha do manequim pertence à **classe**, não ao chassi |

### 9b.2 **Onde a classe carrega o cristal** (a lógica X / Zero / Axl)

**A referência do autor:** em Mega Man X, o X tem o cristal **vermelho**, o Zero tem na **testa** (verde) e o Axl
também na testa (**azul**). Ou seja: **o corpo é um só, e o cristal fica num lugar do corpo** — e *quem* é o
personagem se lê por **onde** o cristal está, não só pela cor.

Isso cria **duas identidades independentes no mesmo personagem**, as duas visíveis de cima:

| Eixo | O que informa | Quem decide |
|---|---|---|
| **Cor do núcleo** | *o que* o Runner faz (frio, crítica, éter, silêncio…) | **o chassi** (§9b) |
| **Lugar do núcleo** | *quem* o Runner é (a classe, o corpo) | **a classe** |

**Proposta de colocação por classe** (6 posições distintas):

| Classe | Onde o cristal mora | Por quê |
|---|---|---|
| **Blaster** | **na testa**, sob a viseira | é quem mira: o cristal é o olho (a lógica do X) |
| **Breaker** | **no peito**, em casulo blindado | provocação: o tanque **mostra** o núcleo — "vem pegar" |
| **Blade** | **no antebraço da arma** | duelista: o núcleo vive onde o golpe nasce |
| **Support** | **nas costas da mão** | quem cura oferece a mão — e o cristal fica na face que o mundo vê quando ele estende o braço |
| **Engineer** | **na omoplata / doca dorsal** | é de onde os drones decolam |
| **Sentinel** | **no braço do escudo** | o cristal é o emissor do campo que defende |

> **Alerta de leitura (câmera alta):** com a câmera do jogo, o que aparece bem são **peito, ombros, dorso e topo
> do capacete**. Testa lê parcialmente; **palma da mão só lê quando o Support abre a mão** (ataque, cura, emote).
> Isso não invalida a escolha — dá **leitura situacional** — mas quem for animar precisa saber que a palma é a
> posição de menor visibilidade e que o Support deve ter um gesto de exposição (por exemplo, ao conjurar).

**Prompt do chassi (cérebro-cristal, SOZINHO — decisão do autor):**

```text
[STYLE ANCHOR]
A RUNNER CHASSIS. It is NOT a robot, NOT a humanoid, NOT a body: it is a small crystal brain - a faceted
self-lit crystal core about the size of two fists - floating inside a VISIBLE AURA that behaves like wind:
a luminous formation like the ice-bloom that grows around frozen surfaces, a cold glowing wind circling the
crystal. AURA COLOUR: <cor do núcleo do chassi, §9b>. AURA FORM: <form da aura pela Body do DataTable>.
No arms, no legs, no head, no face, NO cradle and NO housing: the crystal and the aura are the whole object,
on their own. Plain mid-gray background, the aura in motion, museum-reliquary lighting.
```

## 9c. **Humanização e apresentação** (adendo do autor) — **isto é da CLASSE**

> ⚠️ **Correção de escopo:** a apresentação (Masculine / Feminine / MaleFem / Femasc / Androgynous) **não pertence
> ao chassi** — o chassi é a mesma coisa para todo mundo. **É a classe que define o corpo**, e é aí que a
> apresentação entra. A tabela de forma e rosto abaixo continua valendo; o que muda é **onde ela é aplicada**.

**Referência de estilo do corpo do jogador (autor):** `Art/reference/TronRoll3.jpg` e as amostras de
`Art/Classes/` — gente com **rosto, expressão, cabelo e anatomia crível**, desenhada com calor. *"Se ficar muito
mecânico como os da imagem gerada, não presta para ser os players."* Ou seja: o estilo robótico e sem rosto está
**proibido** para corpo de jogador — ele é a linguagem dos **Apagados** (§9d).

**O que o autor apontou:** os chassis gerados estão **robotizados demais** — sem corpo humano, sem rosto, sem
gênero. Num mundo de avanço tecnológico, a expectativa é o contrário: são **pessoas** com hardware, e o elenco
deve cobrir um **espectro de apresentação**, não um binário.

### As cinco apresentações

Regra: a apresentação se comunica por **silhueta e proporção**, nunca por fantasia (nada de "armadura com
símbolo de gênero"). Cada categoria tem assinatura de corpo e de rosto:

| Apresentação | Silhueta e proporção | Rosto |
|---|---|---|
| **Masculine** | ombros claramente mais largos que o quadril, tronco em V, pescoço grosso | mandíbula marcada, sobrancelha pesada |
| **Feminine** | quadril ≥ ombros, cintura marcada, pescoço longo, mãos pequenas | mandíbula suave, olhos grandes |
| **MaleFem** (corpo masculino que lê feminino) | estrutura de ombros masculina com cintura fina, quadril um pouco mais largo e postura suave | traços finos dentro de uma moldura larga |
| **Femasc** (corpo feminino que lê masculino) | estrutura feminina com ombros e braços musculosos, pulsos grossos | traços duros em crânio estreito |
| **Androgynous** | ombro ≈ quadril, sem marcador binário, linhas equilibradas | traços neutros, ambíguos de propósito |

### A regra de humanização (o que tirar e o que pôr)

| Tirar | Pôr |
|---|---|
| cabeça de robô sem rosto, viseira que cobre tudo | **rosto humano** com olhos e expressão legíveis; viseira é **acessório**, não substituto de rosto |
| corpo todo em placa, sem anatomia | **anatomia humana sob o hardware**: pescoço, cintura, articulação de quadril e ombro, mãos com dedos |
| "armadura que é o corpo" | hardware **vestido sobre um corpo vivo** — pele sintética pode aparecer no pescoço, mandíbula e mãos |
| cabeça lisa e igual em todos | **cabelo ou forma de crânio própria** por chassi (é o marcador humano mais rápido) |

> O canônico já vai nessa direção: as amostras de classe em `Art/Classes/` têm rosto, expressão e
> leitura de gênero. E o projeto **já tem as duas malhas humanas da engine** (`SKM_Manny_Simple` masculino e
> `SKM_Quinn_Simple` feminino) — hoje o DataTable aponta **todos os 11 chassis** para o Manny.

### Proposta de distribuição (2 por apresentação, 10 chassis)

| Chassi | Apresentação | Malha de placeholder |
|---|---|---|
| **Charger** | Masculine | Manny |
| **Forgekin** | Masculine | Manny |
| **Vox** | Feminine | Quinn |
| **Aetheric** | Feminine | Quinn |
| **Droneframe** | MaleFem | Manny |
| **Techno** | MaleFem | Manny |
| **Cryonix** | Femasc | Quinn |
| **Overcore** | Femasc | Quinn |
| **Vitaspark** | Androgynous | Quinn |
| **Ghostnet** | Androgynous | Manny |
| *Clyffen (extinto)* | Androgynous | — |

**O que isso exige do código e dos dados:** uma coluna nova no `DT_Chassis` (apresentação) e um campo no
`FRunnerChassisData`; a malha por chassi passa a ser Manny ou Quinn conforme a tabela acima, o que dá distinção
real em campo **hoje**, com asset da própria engine.

## 9d. **Os Apagados** — a família corrompida (para onde foi a arte "robótica")

**A decisão do autor:** os chassis gerados antes do adendo de humanização (robotizados, sem rosto) **não são
desperdício — são os Apagados**. E isso cai exatamente no canônico: o GDD §2.6 diz que os Apagados são *"os
tomados pela névoa — o que sobra deles continua lá dentro, e às vezes sai"*.

Ou seja: o que estava **errado como personagem jogável** (oco, sem rosto, mecânico, sem gente dentro) é
**exatamente certo** como inimigo. A cara de casca vazia deixou de ser defeito e virou a regra da família.

### Regras visuais dos Apagados

| Regra | Como aparece |
|---|---|
| **Casca oca** | as placas têm brechas que mostram **interior vazio** — cabos soltos, nada dentro. Não é corpo ferido: é corpo que **não está mais lá** |
| **Sem rosto** | placa facial lisa ou viseira **morta/preta**; às vezes **um** optivo piscando (nunca os dois) |
| **Luz errada** | o núcleo de identidade continua na cor daquele chassi, mas **falhando**: piscando, meio apagado, vazando pelas fissuras. É o que faz o jogador reconhecer *"isso já foi um Vitaspark"* |
| **Ferrugem e remendo** | oxidação e **peças trocadas entre chassi** — braço de Charger no torso de uma Vox. Eles são feitos de sobras, e isso conta a história sem texto |
| **Véu nas juntas** | névoa saindo das fissuras (liga a família ao Faden, o Véu do GDD §2.7) |
| **Silhueta quebrada** | proporção humana **quebrada**: um braço grande demais, cabeça torta, perna arrastada. A leitura é "humano, mas não" |

### Onde eles entram na taxonomia (GDD §7.2)

| Categoria | Uso | Origem na arte atual |
|---|---|---|
| **Minion** (enxame) | hordas e corredores: 1 ataque simples, 0 reação | unidades pequenas, no porte do Droneframe |
| **Regular** | patrulhas e setores | as cascas de tamanho humano (a maior parte do que já está gerado) |
| **Elite** | guardas e eventos: 1 reação | uma casca crescida, com membros extras e dois núcleos falhando |

> **Convenção de nome** (segue o `AI_Pipeline/README.md`): `creature_apagado_<nome>_vNNN`. A arte já gerada está em
> `Art/generated/apagados/` (`apagado_<chassi>_v001.png` e `_v002.png`, 20 arquivos), reaproveitada como conceito.

> **Confirmado pelo autor:** a folha v002 (a leva de chassis robóticos, homogênea e sem rosto) **pode ser os
> Apagados corrompidos iniciais** — justamente porque são **muito parecidos entre si** e **nada humanoides**. A
> homogeneidade que reprovava o conjunto como elenco de heróis **aprova** ele como horda: cascas iguais, sem rosto,
> vindas do mesmo lugar.

> **A cor de identidade vale aqui também:** o Apagado mantém a cor do chassi de origem (§9b), porque é isso que faz
> o jogador entender que aquele inimigo **era** um chassi — e a inversão (luz falhando em vez de luz firme) é a única
> diferença entre o herói e a casca. Se as cores forem aprovadas, valem para as duas famílias.

## 9e. **Img2img com as referências** — a receita que funciona (medida, não achada)

Descrever estilo em palavras não cola. O caminho que funciona é **usar as referências do projeto como base** e
deixar o modelo reinterpretar. Ferramenta: `Tools/comfy/img2img_style.py` (local, sem nuvem, sem custo).

**Como ela funciona:** lê o grafo que o ComfyUI executou (`Tools/comfy/workflows/z_image_turbo_base.json`, salvo do
histórico dele — não inventei nó nenhum), troca o latent vazio por `LoadImage` + `VAEEncode`, sobe a referência para
o ComfyUI, enfileira, espera e baixa o resultado. Parâmetro principal: **`--denoise`**.

### O que os três testes ensinaram

| Teste | Referência | Denoise | Resultado |
|---|---|---|---|
| 1 | `Art/Classes/sample_tank.jpg` (robô sem rosto) | 0,60 | **estilo veio, mas veio o robô também** — cor e "natureza" da referência dominam, paleta azul/branca inclusive |
| 2 | recorte do TronRoll3 **sem a cabeça** | 0,72 | paleta do prompt assumiu (latão) e o **cristal apareceu no peito** ✔ — mas **sem rosto**, porque o recorte não tinha cabeça |
| 3 | recorte **com a cabeça** | 0,70 | ✅ **o alvo:** homem com rosto, olhos e cabelo, masculino por proporção, latão e cobre da paleta, cristal âmbar no peito, silhueta de tanque |

**Três regras que ficam:**

1. **Denoise 0,68–0,72 é a faixa útil** para corpo de classe: abaixo disso a referência manda (e se ela é robô, sai
   robô); acima, o prompt manda e o estilo se perde. **0,70 é o ponto.**
2. **O recorte precisa conter a cabeça.** Sem cabeça não há rosto — e sem rosto não existe corpo de jogador (§9c).
3. **O prompt manda na paleta a partir de 0,70.** Foi assim que a referência azul/branca virou latão/cobre com
   âmbar no peito — que é a paleta do projeto.

**Cuidado observado:** aos 0,70 o modelo ainda **arrasta props da referência** (no teste 3 apareceu um hidrante do
desenho vizinho). Solução: recortar mais apertado e acrescentar `no props, no background objects` ao pedido.

## 10. Gramática visual (o que se aprende com o mood board — e o que NÃO se copia)

O mood board interno é `Art/reference/`: folhas de personagem de robô assinadas por artistas
(`Mily.jpg` — "Reaverbot Legends", `Stompy.jpg`, `TronRoll3.jpg`, `TrioTroops.jpg`) mais uma
ilustração clássica do Mega Man. É **referência de gosto**, não acervo: serve para extrair princípios.

| Princípio que o mood board ensina | Como se aplica no Ploidrek |
|---|---|
| **Uma forma forte por personagem** | cada classe lê por UMA silhueta dominante em um segundo, mesmo em preto e branco (tanque = bloco, velocidade = cunha, cura = cápsula) |
| **Proporção heróica compacta** | 6 a 7,5 cabeças, ombros largos, **mãos e botas grandes** — o peso visual mora nas extremidades |
| **Painelização limpa** | chapas grandes, pouca linha de painel, rebite nas junções: o detalhe nunca compete com a silhueta |
| **Cor em 60/30/10** | 60% massa neutra (aço/branco), 30% cor de identidade do chassi, 10% acento emissivo |
| **Capacete é o rosto** | viseira/olhos são a única área expressiva; sem boca, sem nariz |
| **Cel shading limpo** | dois tons de sombra + contorno escuro; especular só no metal |
| **Inimigo pequeno e simpático** | bicho mecânico compacto, um olho ou uma boca — leitura em massa |

**A linha de IP é não negociável — e já é regra sua:** nenhum personagem, nome, logo ou design reconhecível.
O `IP-001`, o `IP-004` e o `MOD-001` do backlog já baniram os termos e mandam derivados para
`Art/reference/_ip/`, sem entrar no build. O mood board é **interno**: inspira proporção, contraste,
painelização e peso de linha — **nunca a forma final**.

**Traduzindo para o nosso jogo:** mantém-se o **gosto** (robô compacto e heróico, capacete expressivo, cel
limpo, inimigo simpático) e troca-se a **identidade**: matéria em latão/cobre/aço da paleta, energia em
ciano/âmbar, capacete com viseira e rebite (não o elmo clássico), 7 cabeças em vez de 7,5, e nomes/narrativa
nossos (Aether Forge · Nexus-7 · Kardys).

> ⚠️ **Nunca vira asset:** `Art/reference/_ip/` (quando preenchida), `Megaman.obj/stl`, `Megaman.fbx`
> `Classes/sample_maverik.jpg`, `generated/world/{reploid,maverick}.png`. São conceito local, e o que eu gero a
> partir delas tem de ser **design original** — é o que os prompts deste documento exigem explicitamente.

## 11. Checklist de aceite (o que eu confiro antes de importar)

- [ ] Resolução e proporção conforme o pedido (e não um upscale disfarçado).
- [ ] Paleta dentro do documento `SteampunkPalette.md` (sem roxo/neon aleatório).
- [ ] Estilo igual às amostras (um asset destoando estraga o conjunto).
- [ ] Textura: **tileable de verdade** (bordas casam) e mapas PBR separados.
- [ ] Personagem: sem corte de pés/mãos, silhueta legível, sem artefato de mão/dedo.
- [ ] Sem texto, marca d'água ou assinatura.
- [ ] Nome do arquivo conforme a convenção (`Art/generated/<familia>/<id>_<tipo>.png`).
