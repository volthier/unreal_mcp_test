# Os 10 Modelos (chassis) — dados reais, nomes e balanceamento

> ℹ️ **Anexo canônico de detalhe.** As **descrições oficiais** dos chassis (texto in-game, já sem termo de IP) e a regra de
> atributos estão em **`Docs/PloidrekRPG_GDD_v3.md` §4**.

> **Fonte:** export JSON do `DT_PloidrekModels` (PloidrekRPG, UE 5.5).
> **Estado:** ✅ dados reais · ✅ vocabulário de tempo · ✅ nomes · 🟡 balanceamento (revisão crítica abaixo)
> **Fora de escopo por ora:** **voo**. A verticalidade passa a ser resolvida por **wall jump / dash** (ver §7).

---

## 1. Dados reais

| # | Row | Bônus | Identidade | Trait | Feature |
|---|---|---|---|---|---|
| 1 | **Charger** | STR +2 · CON +1 | Robusto, força bruta e impacto | **Módulo de Impacto** – dano em área ao aterrissar | +1 ataque corpo a corpo por turno |
| 2 | **Runner → Vitaspark** | DEX +2 | Ágeis, implantes de velocidade extrema | **Hipermotor Cinético** – dash extra por turno | Ignora terreno difícil · escala paredes curtas |
| 3 | **Aetheric** | DEX +1 · CHA +2 | Conectados a energia cósmica/quântica | **Condutor Aether** – resistência a dano de energia | Restaura 1d12 de energia de um aliado (descanso curto) |
| 4 | **Techno** | INT +2 | Núcleos cerebrais voltados à análise | **Analisador de Estruturas** – hackeia portas/inimigos/sistemas | Lê padrões → vantagem ao esquivar por 1 turno |
| 5 | **Droneframe** | DEX +2 | Pequenos, ideais para espionagem | **Propulsores** *(voo suspenso)* | Dificilmente detectável por sensores |
| 6 | **Forgekin** | CON +2 · CHA −1 | Tanques vivos, braços hidráulicos | **Armadura Forjada** – reduz 3 de dano físico por acerto | +5 HP no nível 1 e +1 por nível · carga 1/4 · armas pesadas com uma mão |
| 7 | **Ghostnet** | DEX +2 · WIS +1 | Camuflagem e infiltração | **Camuflagem Ativa** – invisível 1 turno, 1x por descanso longo | Vantagem em procurar · ataque escondido ganha dado extra |
| 8 | **Synthion → Vox** | CHA +2 | Aparência e emoções humanas, influência social | **Módulo Emocional** – inspira aliados (+1d4 em testes) | Vantagem contra medo ou desespero |
| 9 | **Overcore** | DEX +1 | Núcleos de energia sobrecarregados | **Superaquecimento** – a 25 % de vida, pulso em área de 12 m | +2 de dano com armas energéticas · ignora desvantagem de distância |
| 10 | **Cryonix** | CON +2 · INT +1 | Núcleo criogênico | **Nevoeiro Criogênico** – névoa que atrapalha a visão inimiga | Resistência a dano térmico · armas de gelo +1d4 |

---

## 1b. Bônus de atributo — checagem e regra uniforme

**Checagem real** (pedido: "todos deveriam ter um −1"):

| Modelo | Bônus atual | Positivos | Líquido | Tem −1? |
|---|---|---|---|---|
| Charger | STR +2 · CON +1 | +3 | +3 | não |
| Aetheric | DEX +1 · CHA +2 | +3 | +3 | não |
| Ghostnet | DEX +2 · WIS +1 | +3 | +3 | não |
| Cryonix | CON +2 · INT +1 | +3 | +3 | não |
| Vitaspark | DEX +2 | +2 | +2 | não |
| Techno | INT +2 | +2 | +2 | não |
| Droneframe | DEX +2 | +2 | +2 | não |
| Vox | CHA +2 | +2 | +2 | não |
| **Forgekin** | CON +2 · CHA **−1** | +3 | **+1** | **sim (único)** |
| **Overcore** | DEX +1 | +1 | **+1** | não |

**Correção factual:** o Forgekin **não** é o único com +3 — **quatro** modelos têm +3 positivos. Ele é o **único com
penalidade** e, junto com o Overcore, o de **menor líquido**. O Overcore é o mais fraco: +1 e nenhum defeito.

### ✅ Regra final: **o corpo define o defeito**

**Base na criação:** todo Runner começa com **8 em todos os atributos** — **não existe compra de pontos**, e o chassi é a
**única** variável inicial. Sobre essa base, o chassi aplica **+3 positivos** (em até 2 atributos) e **−1 no atributo que o
tipo de corpo cobra**: líquido **+2** para todos.
A lógica é física, então o jogador entende sem ler regra:

| Tipo de corpo | Defeito | Porque |
|---|---|---|
| **Esguio** | **FOR −1** | estrutura fina não carrega peso |
| **Truncoso** | **DEX −1** | massa custa agilidade (**e CA**) |
| **Frágil / pequeno** | **CON −1** | menos HP |
| **Instável** | **SAB −1** | sem autocontrole |

| Modelo | Corpo | Atributos | Líquido |
|---|---|---|---|
| **Charger** | truncoso | STR +2 · CON +1 · **DEX −1** | +2 |
| **Vitaspark** | esguio | DEX +2 · CON +1 · **FOR −1** | +2 |
| **Aetheric** | esguio | CHA +2 · DEX +1 · **FOR −1** | +2 |
| **Techno** | esguio | INT +2 · WIS +1 · **FOR −1** | +2 |
| **Droneframe** | frágil/pequeno | DEX +2 · WIS +1 · **CON −1** | +2 |
| **Forgekin** | truncoso | CON +2 · FOR +1 · **DEX −1** | +2 |
| **Ghostnet** | esguio | DEX +2 · WIS +1 · **FOR −1** | +2 |
| **Vox** | frágil | CHA +2 · WIS +1 · **CON −1** | +2 |
| **Overcore** | instável | CON +2 · DEX +1 · **SAB −1** | +2 |
| **Cryonix** | truncoso | CON +2 · INT +1 · **DEX −1** | +2 |

Distribuição dos defeitos: **FOR −1 ×4** (esguios) · **DEX −1 ×3** (truncosos) · **CON −1 ×2** (frágeis) · **SAB −1 ×1** (instável).

**Dois efeitos que saem de graça e que valem registro:**

1. **Forgekin: o −1 DEX paga o bônus de armadura.** Cada −1 de DES é **−1 de CA**, então o "+2 → +5 de CA" do Forgekin é,
   na prática, **+1 → +4 líquido** — o próprio corpo dele cobra o preço do tanque. E ele **deixa de ser o "feio"** (o −1 CHA
   era arbitrário): agora ele é **o lento**, que é o que ele é.
2. **Overcore: SAB −1 faz dele o chassi mais vulnerável ao Fadenclyffe.** A "puxada" no Véu Profundo é **salvaguarda de
   Sabedoria** — logo, o chassi instável é o primeiro a **caminhar para dentro da névoa**. O defeito dele não é só número:
   é um destino.

---

## 2. Vocabulário de tempo (fechado)

| Termo | Valor |
|---|---|
| **Rodada** | 6 s |
| **Turno** | 10 rodadas = 60 s (o "minuto" do D&D 5e) |
| **Descanso curto** | 10 min fora de combate |
| **Descanso longo** | 20 min |

⚠️ Todo trait que diz *"por turno"* significa **por 60 s**.

---

## 3. Nomes

| Row original | Vira | Motivo |
|---|---|---|
| `Synthion` | **Vox** | chassi de influência (CHA +2); a habilidade usa bônus de CAR |
| `Runner` | **Vitaspark** | chassi de velocidade (DEX +2); libera "Runner" para o povo |

**Sobre "Vitaspark":** funciona na família de nomes compostos (Forgekin, Ghostnet, Overcore, Droneframe). O único senão é
semântico — *vita* lê como "vida/vitalidade", não como velocidade. Alternativas que mantêm o estilo: **Vitesse (Vitesse-9)**,
**Corisco**, **Voltspark**, **Zephyr**.
**`Runner` = o povo** · **`N.E.R.V.` = o núcleo** — travado.

---

## 4. Metodologia do balanceamento

Requisito declarado: **as vantagens de modelo têm que ser todas convidativas e de potência comparável**.

**Unidade de Vantagem (UV)** — orçamento único para comparar coisas de naturezas diferentes:

| 1 UV equivale a | Exemplo |
|---|---|
| **+1d4 (≈2,5) de dano por acerto**, em condição clara | +1d4 nos ataques com o drone ativo |
| **+1 de CA condicional** | +2 CA enquanto não se mover |
| **1 uso limitado de ferramenta exclusiva por combate** | silenciar, drenar Éter, pulso de área |
| **≈25 % de um recurso por combate** | restaurar 1d12 de Éter |

**Regra de orçamento:** **Primária = 2 UV · Secundária = 1 UV · total 3 UV por chassi.**
**Regra de contrapeso:** quanto mais fácil a condição, menor o valor. Condição trivial (flanquear) **não** pode pagar
bônus fixo alto.

---

## 5. Diagnóstico crítico da proposta (sincero)

**Veredito: sim, ainda está desbalanceado — e o problema é estrutural, não de números isolados.** Os cinco piores:

| # | Chassi | Problema | Impacto |
|---|---|---|---|
| 1 | **Forgekin** | **+5 CA** pela melhor característica, permanente | CA extra funciona contra **todo** ataque, para sempre. +5 CA ≈ −25 % de dano recebido **em todas as rodadas** — é o item mais forte de todo o jogo, mais forte que qualquer habilidade de classe |
| 2 | **Vitaspark** | +3 de dano **por carga** sem dizer que a carga é **consumida** | Com 4 ataques/rodada e 6 cargas = **+18 em cada acerto** (~+43/rodada). Quebra o combate |
| 3 | **Charger** | Flanqueio (condição trivial) pagando **+4 fixo por acerto** | ~+9,6/rodada, e escala com o nº de ataques, não com o nível. No 5e, flanquear dá **vantagem**, não dano |
| 4 | **Droneframe** | Drone como **reação extra** | Reação é 1/rodada no jogo inteiro; uma segunda reação **dobra a defesa** (ou o ataque, se for contra-ataque) |
| 5 | **Aetheric** | **Regeneração de Éter dobrada** = +15 Éter/rodada | Anula o custo das habilidades — e o Éter foi desenhado para ter peso a vida inteira |

**Três erros técnicos que aparecem em vários chassis:**

1. **CD = 8 + prof + mod + `nível`** (Charger e Cryonix). O d20 com bônus máximos chega a ~+11; somar o nível ao CD cria
   CDs impossíveis no 10 e irrelevantes no 2. Use **CD = 8 + proficiência + modificador** (o nível já entra pelo bônus de
   proficiência).
2. **"Vantagem de dano"** não existe no 5e — vantagem é sobre rolagens de d20. Precisa escolher: **vantagem no ataque**
   (2d20) **ou** dano extra nomeado.
3. **Números fixos por acerto escalam com o nº de ataques por Ação de Ataque** (1 → 4 por nível). Todo bônus fixo precisa
   ou ser **por rodada**, ou ser **consumido**, ou virar **dado**.

---

## 6. Balanceamento ajustado (proposta)

| Chassi | Primária (2 UV) | Secundária (1 UV) |
|---|---|---|
| **Charger** | **Investida Sísmica**: atinge o alvo central do cone; os adjacentes fazem salvaguarda de **FOR — CD 8 + prof + mod FOR** (sem +nível). 1x por combate. Quem falha cai e fica **marcado**: o Charger tem **vantagem** contra marcados até o fim do combate | **Flanqueador**: flanquear concede **vantagem** (regra 5e) e **+2 de dano no primeiro acerto de cada rodada** |
| **Vitaspark** | **Crítico Ampliado**: **crítico em 18–20 somente com as 6 cargas** (nada no meio do caminho) | **Carga Cinética**: **+3 de dano por carga** (máx 6 = **+18**), **consumido no acerto**. Cargas: **máx 6** · **+1 por rodada em que se move** (+1 extra se usar dash) · parar = **−1** · ser acertado = **−2** |
| **Techno** | **Protocolo de Deflexão**: a reação de esquiva concede **+5 de CA até o fim da rodada**; **3 usos**, recarrega no descanso longo | **Analisador de Estruturas**: hackeia terminais, portas, drones e sistemas inimigos — **acesso** (casa com o gating por Data Core) |
| **Aetheric** | **Restauração de Éter**: **1d12** ao toque (si mesmo ou aliado adjacente), **2 usos por combate** | **Condutor**: **+50 % de regeneração de Éter** (não dobrado — dobrar mata o peso do recurso) |
| **Droneframe** | **Drone de Apoio**: **+1d4 de dano** nos seus ataques enquanto o drone estiver ativo. Drone: **HP = 1/6 do HP do personagem**, **CA = a do personagem**; **recolher/reinvocar = 1 ação bônus**; se for **destruído**, reimplantação exige **descanso longo dobrado (40 min)**. *Voo suspenso por ora* | **Interceptação**: **1x por rodada**, o drone executa a **reação** por você |
| **Forgekin** | **Armadura Forjada**: reduz **3** de dano físico por acerto · **+5 HP no N1 e +1 por nível** | **Blindagem Adaptativa (escala com o nível)**: **+2 CA (1–6) · +3 (7–12) · +4 (13–18) · +5 (19–20)**. É **bônus de armadura** — **não acumula** com escudo/campo de energia |
| **Ghostnet** | **Camuflagem Ativa**: invisível por **3 rodadas**, ou até atacar/sofrer dano; 1x por descanso longo | **Dreno de Núcleo**: **3 usos**, troca o dano do ataque por **dano no Éter (1:1)**, só contra alvos com Éter; recarrega com **10 min** fora de combate |
| **Vox** | **Voz Nula**: silencia e drena Éter em raio de **6 m** — duração **1d4 + mod CAR rodadas**, **1x por combate**, recarrega no descanso longo; recupera **2 × (rodadas silenciadas) por inimigo afetado** | **Módulo Emocional**: 1x por combate, por **2 turnos (120 s)**, suas habilidades e as de aliados no raio ganham **+1d4** |
| **Overcore** | **Superaquecimento**: a 25 % de vida, pulso em área de **12 m**, dano = **min(¼ do HP perdido, nível + mod CON)** | **+2 de dano com armas energéticas** e **ignora a desvantagem de distância** |
| **Cryonix** | **Lentidão Criogênica**: até **3 inimigos**, salvaguarda de **INT — CD 8 + prof + mod INT**; quem falha passa a gastar **o dobro do tempo em qualquer ação dirigida ao Cryonix** (ataque com cadência de 1 s vira 2 s; habilidade que gasta 1 ação gasta 2) por **3 rodadas**; 1x por combate, recarga no descanso longo | **Nevoeiro Criogênico**: área que impõe **desvantagem** à visão inimiga por **2 rodadas** |

**Fica equilibrado agora?** Muito mais perto. O que mudou de fato: (a) os dois bônus de dano fixo viraram **condicionais e
consumidos**; (b) o +5 CA virou **+3 posicional**; (c) a reação extra virou **ferramenta do drone, com 1 uso por rodada**;
(d) o Éter dobrado virou **+50 %**; (e) as CDs saíram do +nível.

---

## 7. Mobilidade

Movimento, verticalidade e mouse estão especificados em **`Docs/GDD_v3_Fundacao.md` §Mobilidade** (fonte única). Resumo:
voo suspenso; dash vai para o cursor; wall slide + wall jump destravados por **Data Core**; cada movimento consome o
**slot de Mobilidade (1 por rodada)**; terreno alto entrega **Vantagem** no ataque — que é a moeda do d20, não um número novo.

---

## 8. Pendências

| # | Item | Tipo |
|---|---|---|
| 1 | ✅ **Vitaspark**: "+6" = **teto de 6 cargas**; crítico progressivo proposto (4 → 19–20 · 6 → 18–20) | ✅ |
| 2 | ✅ **Aetheric**: **+50 %** de regeneração confirmado | ✅ |
| 3 | ✅ **Forgekin**: **+2 → +5** escalando por nível (não acumula com escudo) | ✅ |
| 4 | ✅ **Cryonix**: lentidão = **dobro do tempo em ações dirigidas a ele** | ✅ |
| 5 | ✅ **Overcore**: limite = **nível + modificador de CON** | ✅ |
| 6 | ✅ **Drone**: HP 1/6 do personagem · CA = a do personagem · reimplantação = descanso longo dobrado | ✅ |
| 7 | ✅ **Vitaspark**: crítico **só com as 6 cargas** (18–20) — sem progressivo | ✅ |
| 8 | ✅ **Drone**: recolher = 1 ação bônus; só a **destruição** custa os 40 min | ✅ |
| 9 | ✅ **Tomados = os Apagados** | ✅ |
| 9b | ✅ **Regra de bônus uniforme** (+3 positivos e −1, líquido +2) sobre **base 8 em tudo** | ✅ |
| 10 | ✅ **10 descrições reescritas** — texto oficial em `Docs/PloidrekRPG_GDD_v3.md` §4; falta só aplicar no DataTable do projeto | ✅ |
