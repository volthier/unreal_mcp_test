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
