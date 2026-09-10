# Modelo de interação — como o jogador AGE no PloidrekRPG

> **Por que este documento existe:** o GDD v3 cobre muito bem **as regras** (D&D/5e, d20, janela de rodada de 6 s)
> e **o mundo** (Nexus-7, 3D contínuo, sem tela de fase). O que **não estava escrito em lugar nenhum** é o que fica
> entre os dois: **como o jogador anda, mira, ataca e percebe o combate**. Sem isso, arte e código divergem — e já
> divergem hoje (o pawn em C++ é terceira pessoa de controle direto, o que contradiz o modelo descrito abaixo).

**Fonte da intenção (palavras do autor):** *"o jogo é 3D, clica para andar que nem LoL e ataque também, mas com a
jogabilidade de Megaman e regras de DnD"*.

**Estado da arte (verificado):**

| Tema | Documentado? | Onde |
|---|---|---|
| Regras 5e, HP/CA, d20, proficiência | ✅ | `PloidrekRPG_GDD_v3.md` §3 e §6 |
| Janela de Rodada (6 s, tick 0,5 s), salvaguardas | ✅ | GDD §6.1 e §6.2 |
| Mobilidade e verticalidade (wall jump, sem voo, 1 por rodada) | ✅ | GDD §6.4 |
| Mundo aberto **3D** contínuo | ✅ | GDD §1.2 |
| Identidade visual (latão/cobre/ferro + **neon funcional** + céu dusk) | ✅ | GDD §11 + `SteampunkPalette.md` |
| **Clique para andar/atacar, câmera, mira** | ❌ **nada** | — |
| **Como a janela de 6 s convive com tempo real** | ❌ **nada** | — |
| **"Feel" de Mega Man** (salto, cadência, leitura de padrão) | ❌ **nada** | — |
| Perspectiva de jogo (ângulo, distância, rotação) | ❌ **nada** | — |

## 1. O modelo (autor) × o que o código faz hoje

| | Modelo descrito | Código hoje (`ARunnerCharacter`) |
|---|---|---|
| Andar | **clique no chão** (estilo LoL) | `AddMovementInput` (WASD/analógico) |
| Atacar | **clique no alvo** + habilidades | nenhum ataque implementado |
| Câmera | **ângulo alto fixo** sobre o personagem | `CameraBoom` + `FollowCamera` de terceira pessoa |
| Regras | **d20, janela de 6 s** | nada ligado ao combate |

> Ou seja: **o código atual não é o jogo descrito** — ele é um scaffold que veio do renome do VoltStriker (o próprio
> GDD §15.1 já registra que a malha era teste de pipeline e o scaffold GAS é reaproveitável).

## 2. Câmera — decisão 1

**Proposta (não canônica até você confirmar):** câmera **de ângulo fixo** sobre o Runner, não perseguição colada.

| Parâmetro | Valor proposto | Por quê |
|---|---|---|
| Inclinação | **50° a 55°** | lê o chão (área de habilidade, telegraph) e ainda mostra o volume do corpo |
| Distância | **10 a 14 m** | cabem o Runner, dois aliados e o grupo inimigo no quadro |
| Rotação | **livre pelo mouse (arrastando) + snap em 45°** | orientação tática sem virar jogo de voo |
| Zoom | 2 passos (perto/longe) | leitura de detalhe × consciência de área |

**Consequência para a arte:** com câmera alta e distante, o que informa o personagem é **silhueta e cor** (ombros,
capacete, tinta do chassi) — detalhe fino de superfície quase não aparece. Isso muda a prioridade dos assets: primeiro
**silhueta legível em ângulo alto**, depois textura.

## 3. Andar e atacar por clique — decisão 2

| Ação | Entrada | Comportamento |
|---|---|---|
| Andar | clique (e segurar) no chão | `MoveToLocation` em NavMesh; alvo marcado no chão |
| Andar no ar / verticalidade | Space (pulo), Space+parede (wall jump), Shift (dash) | **é aqui que mora o feel de Mega Man**: pulo e dash continuam sendo controle direto, mesmo com o resto no clique |
| Ataque básico | clique sobre o inimigo | disparo/ação básica da classe; sem mira manual — o alvo é o que está sob o cursor |
| Habilidades | teclas 1 a 4 | algumas miram **no ponto** (clique no chão), outras no **alvo** (clique no inimigo) |
| Cancelar | Esc | cancela a habilidade armada |

**Regra de ouro proposta:** *andar é clique, agir é clique, mas a **mobilidade de precisão é tecla*** — é o que permite
ter MOBA na locomoção e Mega Man no movimento.

## 4. Janela de rodada × tempo real — decisão 3 (a mais delicada)

Aqui os dois referenciais brigam: LoL é contínuo, D&D é por turnos. **Proposta:**

- o combate corre **em tempo real**, mas o tempo é **fatiado em janelas de 6 s** com **tick de 0,5 s** (as regras do
  GDD §6.1 já são escritas assim): cada criatura tem, por janela, **1 Ação de Ataque, 1 Movimento, 1 Reação**;
- o jogador **não espera a vez**: ele gasta o orçamento **quando quer** dentro da janela; o que acabou acende no HUD;
- o **d20 resolve o acerto** na hora do clique (não no fim da janela): o feedback é imediato — dano, erro, crítico;
- o **inimigo** também joga na mesma cadência (a IA gasta o orçamento dela na janela, com telegraph);
- **descanso curto/longo** (10/20 min diegéticos) recuperam recursos entre janelas de combate, não durante.

Isso preserva as regras (você continua jogando 5e) sem transformar o jogo em xadrez parado.

## 5. "Feel" de Mega Man (requisitos mensuráveis)

Sensação não se escreve com adjetivo. Traduzido em números para o código conseguir mirar:

| Sensação | Número proposto |
|---|---|
| Pulo com peso e leitura | altura **2,2 m**, tempo total no ar **0,75 s**, gravidade assimétrica (sobe mais lento que cai) |
| Dash seco | **4 m em 0,2 s**, sem inércia no fim |
| Tiro responsivo | do clique ao projétil: **≤ 80 ms** de animação antes do spawn |
| Impacto com presença | **hitstop de 60 a 90 ms** + flash de 1 quadro no atingido |
| Leitura de padrão (inimigo) | telegraph de ataque **≥ 0,4 s**, cor de aviso única (âmbar da paleta) |
| Wall jump | encadeável, **1 por parede**, devolve 80% da altura do pulo |

## 6. O que isso muda no código (quando for implementar)

| Hoje | Vira |
|---|---|
| `ARunnerCharacter` terceira pessoa, `CameraBoom`+`FollowCamera` | câmera em ângulo fixo (SpringArm próprio, sem controle direto de câmera no WASD) |
| `AddMovementInput` | `PlayerController` com **NavMesh** + `MoveToLocation` por clique, com marcador no chão |
| nenhum ataque | **clique no alvo** → `AbilitySystemComponent` (o scaffold GAS já existe) dispara a habilidade |
| HUD: nenhum | barra de habilidades (1-4), indicador de orçamento da janela, retículo/marcador de alvo |
| sem noção de janela | `RunnerCombatWindow` (6 s / 0,5 s) alimentando o HUD e a IA |

## 7. Decisões abertas (eu preciso de você)

1. **Câmera**: concorda com o ângulo fixo alto (50-55°, 10-14 m) ou quer perseguição colada (o que está no código)?
2. **Tolerância a tempo real**: as habilidades entram por **tecla** com mira por clique (proposta) ou tudo por clique?
3. **Mobilidade de precisão**: pulo/dash no **teclado** (proposta, para o feel de Mega Man) ou também por clique?
4. **Pausa**: combate **nunca pausa** (proposta, é o modelo de ação) ou abre micro-pausa na escolha de habilidade?
