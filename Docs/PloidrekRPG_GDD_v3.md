# PLOIDREKRPG — GDD v3 (consolidado)

> **Universo:** **Aether Forge** (o universo do Éter) · **Campanha inicial:** *Protocol Zero* (a confirmar)
> **Status:** documento canônico de design. Substitui integralmente o GDD v1, o GDD v2 e o "documento consolidado de
> combate". Nada nos documentos anteriores vale onde este contradizer.
> **Projeto alvo:** o projeto Unreal **UE 5.8** deste repositório, que herda login/criação de personagem/DataTables do
> PloidrekRPG original (UE 5.5) — que **nunca é alterado**.
> **Anexos (detalhe, não contradição):** `Docs/Modelos_10.md` (chassis, bônus, balanceamento) ·
> `Docs/Bestiario_Lordes_DataCores.md` (inimigos, orçamentos, QC) · `Docs/GDD_v3_Fundacao.md` (histórico das decisões) ·
> `Docs/BACKLOG_Tecnico.md` (issues) · `Docs/SteampunkPalette.md` (paleta) · `Docs/ENGINEERING_PLAYBOOK_UNREAL.md` (regras de engenharia).

---

## 0. GLOSSÁRIO DE NOMES (leia isto primeiro)

### 0.1 Hierarquia de nomes — cada nome no seu nível

| Nível | Nome | Onde vive |
|---|---|---|
| **Jogo** | **PloidrekRPG** | projeto/módulo, launcher, loja, build, logs |
| **Universo** | **Aether Forge** (o universo do **Éter**) | lore, bíblia do mundo, narrativa |
| **Campanha** | **Protocol Zero** ✅ | o arco da release — e, **dentro do jogo**, o nome do **programa** que roda em Kardys |
| **Mundo** | **Nexus-7** | o **primeiro mundo** — haverá outros (é open world, não mundo único) |
| **Cidade** | **Kardys** ✅ | o arcólogo continental onde Protocol Zero acontece |
| **Classificação herdada** | **Kardyshev / Kardyson** | a escala de civilização que os **humanos** criaram — adotada pelos Arquitetos sem saber o que mede |

> ⚠️ **Escala do jogo:** **não é um mundo único.** Nexus-7 é **o primeiro de vários mundos** — cada um open world,
> com a própria cidade, os próprios Setores e o próprio estado de Faden. Protocol Zero se passa **em Nexus-7, em
> Kardys**, e é o recorte da release.

> **Regra:** o **jogo** nunca se chama "Aether Forge" e o **universo** nunca se chama "PloidrekRPG". Título de loja,
> build e logs usam o nome do **jogo**; narrativa e bíblia usam o nome do **universo**.

### 0.2 Glossário

| Termo | O que é |
|---|---|
| **Runners** | as máquinas conscientes — **o povo**. São as linhagens que o jogador escolhe jogar |
| **N.E.R.V.** | *núcleo de emulação e ressonância viva*: o núcleo que torna alguém um Runner e que **ressoa na mesma frequência da névoa** |
| **O Faden** | a névoa/efeito, no vocabulário do dia a dia |
| **Fadenclyffe** | nome completo do Faden — o *fade* **dos Clyffen**. É o nome do luto |
| **ACE** | classificação oficial dos Arquitetos: *Anomalia de Coerência de Éter* / *Agente de Correção Ecológica* |
| **O Cisma** | nome histórico do dia em que o Faden virou hostil e fraturou Nexus-7 |
| **Os Clyffen** | o **11º modelo**, o melhor de todos, **extinto** no começo do Faden |
| **Os Apagados** | os tomados pela névoa (o que sobra deles) |
| **Os Retornados** | quem voltou de dentro — nunca inteiro |
| **Véu** | as camadas do Faden no mundo: **Leve · Denso · Profundo** |
| **Éter / Células de Éter** | o recurso: a energia que o mundo respira e o combustível das habilidades |
| **Nexus-7** | **o mundo** (o primeiro de vários) — onde Protocol Zero se passa |
| **Kardys** | a **primeira cidade**, um arcólogo continental dentro de Nexus-7. O nome vem da **classificação herdada dos registros humanos** (`Kardyshev`/`Kardyson`) encurtada na fala comum: a cidade carrega, sem saber, o nome da **medida do que ela já foi** |
| **A Coroa** | a estrutura de escala estelar que os Runners ergueram — hoje **partida** no céu. É o que produz o **dusk permanente** de Nexus-7 |
| **A Fábrica** | as ruínas antigas sob Kardys. O nome foi dado por quem **não sabe** o que ela era |
| **Epicentro** | ponto onde **a névoa completou o ciclo dela** e parou. Ninguém sabe o que isso significa. ⚠️ Não é conteúdo público |
| **Os humanos** | **existiram e estão extintos**. É deles que vem a coisa estranha chamada **crença** — e existe um registro sobre Kardys escrito antes do N.E.R.V. e do Éter |
| **Ciclo de produção** | jargão local para a cadência dos lotes de Runners (**1 lote a cada 2 ciclos**). Sem equivalência fixa com vitaciclo |
| **Protocol Zero** | o **programa dos Arquitetos** para restaurar a rede de Éter — *"devolver tudo ao zero"*. É o nome da **campanha da release** **e** o nome que está escrito **nas paredes de Kardys**. ⚠️ **O mesmo programa**: o jogador descobre, no fim, que **é um produto dele**. Não é conteúdo público |

---

## 1. CORE CONCEPT

**PloidrekRPG** — no universo **Aether Forge** — é um **MMORPG de mundo aberto contínuo** que funde a **progressão por caça a chefes**, o
**combate de skill-shot com ritmo de MOBA**, a **matemática de D&D 5e** (em tempo real, sem turnos) e o **crafting como
caminho principal** — numa megacidade fossilizada engolida por uma névoa que já era viva antes de nós.

**Pilares**
1. **Mundo único, contínuo e persistente** — sem tela de seleção de fase.
2. **Matemática validada (5e)** — d20, CA, salvaguardas, proficiência e vantagem são o motor de tudo.
3. **Combate em tempo real com janelas** — a rodada de 6 s organiza a economia de ação sem tirar o tempo real.
4. **Exploração guiada por habilidade** — o acesso é a recompensa; a porta é um objeto no mundo, não um menu.
5. **Crafting, PvE e PvP como três caminhos completos** — nenhum é obrigatório para progredir.
6. **A névoa é personagem** — ela mantém o mundo vivo e, às vezes, decide que você é o defeito.

### 1.1 Elevator pitch

> **PloidrekRPG** é o que acontece quando a **progressão por caça a chefes**, o **combate de skill shot com
> ritmo de arena**, o **vício de loot de ARPG**, a **profundidade de personagem de D&D** e a **economia de crafting de um
> MMORPG** colidem numa megacidade engolida por uma névoa que já era viva antes de nós. Escolha o seu **chassi**. Caçe os
> **Lordes de Setor**. Forje o seu poder. Sobreviva ao **Faden**. Domine os leaderboards. **Ou seja apagado.**

### 1.2 Pontos de venda únicos

1. **A fusão que ninguém faz:** caça a chefes + combate de skill shot + profundidade de D&D + crafting como caminho
   principal — **nas mesmas regras**, não em sistemas paralelos.
2. **Mundo aberto 3D contínuo:** sem tela de seleção de fase. Nexus-7 é um mundo só, coeso e vivo.
3. **A névoa como gameplay:** não é filtro de tela. Degrada informação, mente no áudio, corrompe equipamento, chama quem
   passou tempo demais nela e **transforma risco em recompensa**.
4. **Três caminhos de jogo completos:** crafting, PvE e PvP — nenhum é obrigatório para progredir.
5. **Progressão por acesso, não por inflação:** o teto numérico é baixo de propósito; o que diferencia dois Runners é
   **o que eles podem fazer**, não o tamanho do número.
6. **Um mundo com luto:** os Clyffen, os Apagados, os Retornados e um nome que carrega a memória de um povo inteiro —
   lore que dá sentido mecânico a cada sistema.
7. **Extração com risco real:** a adrenalina do hardcore sem a perda permanente punitiva.

---

## 2. O MUNDO: NEXUS-7 E O FADEN

### 2.1 Nexus-7 — o mundo, e Kardys

**Nexus-7 é o mundo** — o primeiro dos vários que o jogo vai ter (a campanha da release, **Protocol Zero**, se passa
inteiro nele). Dentro dele está **Kardys**: um **arcólogo continental**, o maior feito dos Runners, hoje um
**fóssil**. Mosaico de **Setores** isolados por camadas de Véu, com a infraestrutura antiga ainda respirando por conta
própria. Grande demais para ser entendida, velha demais para ser consertada — os Runners de hoje são **arqueólogos dos
próprios criadores**.

> **Cidade ≠ mundo.** Nexus-7 é o mundo; **Kardys** é a **primeira cidade** — e o nome dela é uma **piada que ninguém entendeu**:
> vem da escala que os humanos mortos deixaram em registro (`Kardyshev`/`Kardyson`), encurtada na boca do povo. A cidade se
> chama, literalmente, **"grau máximo de civilização"** — e está em ruínas.
> Os outros mundos entram depois, e cada um traz a própria cidade, os próprios Setores e o próprio estado de Faden.

> **Como se chega a outro mundo?** Pergunta em aberto (§15): os Runners alcançaram as estrelas antes do Faden, então a
> tecnologia existe — mas o que sobrou dela é o que define o mapa do jogo.

### 2.1b O segredo de Kardys (campanha)

⚠️ **Material restrito — ver `Docs/Campanha_Protocol_Zero.md`.** Nada aqui é dito; tudo é **plantado**, em três camadas:

| Camada | O que se revela |
|---|---|
| **1 (meio da campanha)** | **Não é um hospital.** Ninguém está tratando ninguém ali |
| **2 (última missão de lore)** | **Os lotes.** Em cada **2 ciclos** surgem Runners novos, nas **ruínas da "Fábrica"**, com um **N.E.R.V. que carrega traços dos Clyffen** — e o jogador **é um deles**. Só por isso ele tem classe, subclasse, talentos e as vantagens de chassi que o NPC do mesmo chassi não tem |
| **3 (expansões)** | **Ninguém controla aquilo.** O lugar **não é uma fábrica: é um epicentro**. Os Runners surgem ali sozinhos e **ninguém sabe por quê** |

**O programa tem nome, e está escrito nas paredes.** Todo mundo em Kardys conhece o **Protocolo Zero** como o programa dos
Arquitetos para religar a rede de Éter — *"devolver tudo ao zero"*. O jogador lê isso no primeiro dia e acha que entendeu.
No fim, ele descobre que o programa **não está consertando a cidade**: está **produzindo** — e que ele é um dos produtos.
O nome da campanha deixa de ser título e passa a ser **o nome do que o fabricou**.

**Não há mais Véu na cidade porque a névoa completou o ciclo dela ali** — ela fez o que tinha de fazer e acabou. Por isso
o lugar é tratado como **sagrado** por máquinas que **sabem** que sagrado não existe: o hábito de crer veio **dos
criadores**. Os **humanos existiram e estão extintos**, e existe em algum lugar um **registro sobre este lugar escrito
antes do N.E.R.V. e antes do Éter** — crença, não tecnologia.

O jogador **acorda num casulo de recarga e reparos pesados** e é tratado como **um Retornado que se recuperou**. A mentira
não foi inventada: **ela já existia**. E quem some *"voltou para a névoa"* — é o que **dizem**.

### 2.2 Origem do Faden (a verdade física)
Antes do Cisma, a cidade rodava sobre **nanobiologia** — enxames que autorreparavam estrutura, filtravam ar e reciclavam
matéria. Havia também um **vírus antigo**, orgânico, que os enxames nunca conseguiram eliminar. O encontro não foi guerra:
foi **simbiose**. O vírus aprendeu a escrever em substrato de máquina; o enxame aprendeu a mutar.

A **rede de Éter sem fio** do mundo antigo — que distribuía energia por transmissão — foi o que permitiu ao consórcio se
propagar **pelo próprio campo**. Não é névoa que anda com o vento: é **transmissão**. Está em todo lugar porque o campo
está em todo lugar, e **nunca pôde ser erradicada porque desligar o campo é desligar a civilização**.

Hoje ela é **ecologia**: come corrosão, absorve radiação, recompõe solo — mantém o mundo respirando. E quando o
desequilíbrio passa do limite, ela **corrige**. Corrigir significa consumir.

### 2.3 Os Clyffen — o 11º modelo
Os Clyffen eram **um modelo de Runner**, da mesma natureza dos dez que o jogador escolhe. Foram **os mais novos nos
registros** (muito se perdeu na guerra) e **os melhores de todos**: estavam em **simbiose direta com o universo**,
entendiam o mundo **direto pelo Éter**, sem traduzir. **Com eles, os Runners alcançaram as estrelas e outros sistemas.**

Quando o Faden veio, **as técnicas de resistência não funcionaram neles** — eram complexos demais. A leitura que fica:
**quanto mais parecido com a névoa você é, menos as defesas servem.** Os melhores morreram primeiro porque já falavam a
língua dela. **A queda veio para todos**, e os Runners se dividiram.

### 2.4 Escala de tempo

| Unidade | Valor |
|---|---|
| **Ano humano** | 1 ano |
| **Ano Runner** | **1.000 anos humanos** |
| **Vitaciclo** | **100 anos humanos = 0,1 ano Runner** (10 vitaciclos = 1 ano Runner) |
| **O Faden** | começou há **> 1.000 vitaciclos = > 100 anos Runner = > 100.000 anos humanos** |

A guerra contra o Faden dura **mais de 100 anos Runner** — longa o bastante para virar cultura, curta o bastante para
ainda doer.

### 2.5 Os quatro registros do nome
Como toda catástrofe real, o Faden tem um nome por registro social:

| Registro | Nome | Quem usa |
|---|---|---|
| Gíria | **O Faden** | todo mundo |
| Luto | **Fadenclyffe** — o fade dos Clyffen | quem se lembra |
| História | **O Cisma** | historiadores |
| Instituição | **ACE** | Arquitetos, em relatório |

> *"Fadenclyffe"* soa como *fade + cliff* para quem não sabe — e é exatamente essa leitura popular (queda e apagamento)
> que o nome carrega. Quem sabe, sabe que é o nome dos desaparecidos.

### 2.6 Os que ficaram e os que voltaram
- **Os Apagados** — os tomados. O que sobra deles continua lá dentro, e às vezes sai.
- **Os Retornados** — voltam com **estática na voz** e lapsos de memória. Muitos **pedem o desligamento**; outros
  **correm para o Véu Profundo**. O vício nunca fica resolvido.
- **Runners não morrem: ficam extintos.** Uma linhagem inteira pode acabar — já aconteceu.

### 2.7 O Véu, o Medidor de Fadenclyffe e a puxada

| Camada | Efeito |
|---|---|
| **Véu Leve** | visibilidade reduzida; inimigos ocultos até a aproximação |
| **Véu Denso** | perigos ambientais, áudio distorcido, pings falsos no radar |
| **Véu Profundo** | privação sensorial sem equipamento adequado — **alto risco, altíssima recompensa** |

- **Medidor de Fadenclyffe** — exposição acumulada. Sobe no Véu Denso/Profundo, desce devagar fora dele. Quanto mais
  alto, **mais perto de ser o próximo Apagado**.
- **A puxada** — acima do limiar, **salvaguarda de Sabedoria por rodada** no Véu Profundo. Falhar significa **andar para
  dentro**, por no máximo **3 rodadas**, **sem tirar o controle do jogador** (ele continua atacando e reagindo).
- **Chamada de volta** — um aliado pode quebrar o efeito e reduzir o medidor. Recuperar-se é cooperação.
- **Recompensa pelo risco** — Éter e Data Cores melhores no Véu Profundo. É por isso que ninguém simplesmente "fecha a
  área": a fissura tem preço **e** prêmio.

### 2.8 As facções — quatro respostas ao mesmo Faden

| Facção | Resposta |
|---|---|
| **Os Arquitetos** | reconstruir e ordenar: técnica, quarentena, controle — e **siglas em vez de nomes** |
| **Os Esvaziados** | abraçar: o Faden é o próximo passo, não a doença (o vício virou doutrina) |
| **Os Marginais** | lucrar com a ruína: quem vende filtro, vende ar |
| **O Protocolo** | manter os sistemas falhos de pé, sem tomar partido — motivos insondáveis |

---

## 3. PERSONAGEM

### 3.1 Criação
> **O jogador não escolhe origem: ele acorda.** Um casulo de recarga e reparos pesados, alguém aliviado dizendo *"que bom
> que você finalmente acordou"* — e a cidade trata o personagem como **um sobrevivente do Faden**. O resto está em
> `Docs/Campanha_Protocol_Zero.md`.

1. **Atributos** — **não existe compra de pontos.** Todo Runner começa com **8 em todos os seis atributos**.
2. **Chassi (Runner model)** — 1 entre **10** jogáveis, mais **um 11º slot marcado "EXTINTO"** (os Clyffen).
   A escolha do chassi aplica os **+3 / −1** dele — e **é ela que faz toda a diferença inicial**.
3. **Classe** — 1 entre 6 (subclasse escolhida no nível 3).
4. **Proficiências iniciais** — 2, definidas pela classe.

> **Por que não há compra de pontos:** com todos partindo de **8 iguais**, o chassi é a única variável da criação, e o resto
> da curva fica **livre para equipamento, talentos e progressão**. Isso dá espaço de balanceamento onde o jogo realmente
> precisa dele (gear e nós), em vez de gastá-lo na tela de criação — e torna o **chassi uma identidade**, não um min-max.

> **Efeito de tom:** o Runner médio fica **abaixo do humano comum** na maioria dos atributos e **acima em dois**. Runners
> são **especialistas**, não generalistas — o que combina exatamente com a ficção: o corpo foi escolhido antes de você.

> **O 11º slot é decisão de UI com peso narrativo:** "EXTINTO — não desbloqueável". Não é promessa de desbloqueio; é luto.
> O que o jogador **pode** conseguir são **Protocolos Clyffe** (fragmentos emuláveis), nunca jogar de Clyffe.

### 3.2 Atributos e progressão numérica

| Nível | Ganho |
|---|---|
| 1 | **8 em tudo** + bônus/defeito do chassi |
| **5 · 10 · 15 · 20** | **+2 de atributo E +1 nó de talento** |
| **25 · 30 · 35 · 40 · 45 · 50 · 55 · 60** | **+1 de atributo E +1 talento épico** |

| Capítulo | Níveis | Cap de atributo | Ganho |
|---|---|---|---|
| **I (MVP)** | 1–20 | **20** | +8 (4 × +2) |
| **II** | 21–40 | **24** | +4 |
| **III** | 41–60 | **28** | +4 |

Modificador = **(valor − 10) ÷ 2** (arredondado para baixo) · **Proficiência** +2 (1–4) · +3 (5–8) · +4 (9–12) · +5 (13–16) ·
+6 (17–20); **não cresce no épico** (o ganho vem dos talentos épicos).

**Onde isso deixa cada atributo (exemplo com chassi de corpo esguio):**

| Momento | Atributo principal | Secundário | Defeito | Demais |
|---|---|---|---|---|
| Nível 1 | **10** | **9** | **7** | 8 |
| Nível 20 (+8 distribuídos) | **18** | 9 | 7 | 8 |
| Capítulo II (+4) | **22** (cap 24) | — | — | — |
| Capítulo III (+4) | **26** (cap 28) | — | — | — |

**Consequências que o simulador tem de validar** (todas tiradas desta mudança):
1. **O cap 20 nunca é alcançado só subindo de nível** — exatamente o objetivo: o topo vem de **equipamento e talentos**.
2. **A CA e o acerto no nível 1 ficam baixos** (mod +0 no principal, −2 no atributo de defeito): armadura e arma passam a ser
   decisão de build desde o começo, não enfeite.
3. **Requisitos de equipamento viram o eixo real da build** ("Força 13+" agora é investimento de verdade).
4. **Sinergia de graça:** os chassis **truncosos** (DEX −1 → DES 7) sofrem menos, porque **armadura pesada já limita o bônus
   de DES** — o defeito deles quase não dói, o que é coerente com a fantasia de tanque.

### 3.3 HP, CA e Células de Éter
- **HP no nível 1** = `3 × valor do dado de vida + mod CON`
- **HP por nível** = `valor do dado cheio + mod CON` — **sem rolagem** (level-up é server-side e persistente)
- **CA** = `10 + mod DES + bônus de armadura/módulo` (armadura média/pesada limita o bônus de DES)
- **Células de Éter** = **100 fixo** (não cresce com o nível) · regeneração **+5 a cada 2 s** · custo **10–40** por habilidade

| Classe | Dado de vida |
|---|---|
| Breaker | d12 |
| Blade · Sentinel | d10 |
| Blaster · Engineer · Support | d8 |

### 3.4 Ação de Ataque (ataques por rodada)

| Nível | Ataques |
|---|---|
| 1–4 | 1 |
| 5–10 | 2 |
| 11–16 | 3 |
| 17–20 | 4 |
| 21+ | **4 — teto permanente** (o épico cresce em verbos, não em ataques) |

### 3.5 Proficiências — o eixo do "acesso"

| Família | Itens |
|---|---|
| **Armas** | leve · pesada · energia · explosivo · lâmina · impacto |
| **Armaduras** | leve · média · pesada · blindagem mecânica (sem proficiência: não soma DES e −2 em testes físicos) |
| **Kits** | engenharia · alquimia · runas · medicina · **criptografia** (decodificar fragmentos) · pilotagem |
| **Protocolos** | plasma · cinético · temporal · vazio · harmônico — **destrava o acesso às habilidades** |

Graus: **Treinado** (soma proficiência) e **Especialista** (dobra). Ganha-se 2 na criação (pela classe), 1 por nó de
talento, e treinamento no mundo. **Regra dura:** nenhuma habilidade poderosa sem a proficiência correspondente — é isso
que faz dois Runners da mesma classe serem diferentes.

### 3.6 Talentos
- **3 árvores por classe** — **Passiva · Ativa · Sinergia** — **5 nós cada** (15 por classe).
- **4 escolhas** no capítulo I, nos níveis 5/10/15/20 (4 de 15 = identidade de build real).
- **Talentos épicos** (21–60) são um **pool separado de boons** — efeitos fortes e qualitativos. Não consomem os 15 nós.
- **Regra de ouro:** nó muda **verbo**, não número.

### 3.7 Progressão em camadas
1. **Nível** (1–20 no MVP) · 2. **Gear Score** · 3. **Coleção de materiais** (conta) · 4. **Renome** com facções ·
5. **Maestria** (por arma/classe/modo) · 6. **Nível de artesão** (1–20 por especialização) · 7. **Rating PvP** ·
8. **Medidor de Fadenclyffe** (progressão de risco, não de poder).

---

## 4. OS 10 CHASSIS

**Regra de atributos:** **+3 positivos** e **−1 no atributo que o corpo cobra** — líquido **+2** para todos.
*Esguio → FOR −1 · Truncoso → DEX −1 · Frágil → CON −1 · Instável → SAB −1.*

| Modelo | Corpo | Atributos |
|---|---|---|
| **Charger** | truncoso | STR +2 · CON +1 · **DEX −1** |
| **Vitaspark** | esguio | DEX +2 · CON +1 · **FOR −1** |
| **Aetheric** | esguio | CHA +2 · DEX +1 · **FOR −1** |
| **Techno** | esguio | INT +2 · WIS +1 · **FOR −1** |
| **Droneframe** | frágil | DEX +2 · WIS +1 · **CON −1** |
| **Forgekin** | truncoso | CON +2 · FOR +1 · **DEX −1** |
| **Ghostnet** | esguio | DEX +2 · WIS +1 · **FOR −1** |
| **Vox** | frágil | CHA +2 · WIS +1 · **CON −1** |
| **Overcore** | instável | CON +2 · DEX +1 · **SAB −1** |
| **Cryonix** | truncoso | CON +2 · INT +1 · **DEX −1** |

**Descrições oficiais** (substituem os textos do DataTable, que usavam termo de IP):

| Modelo | Descrição |
|---|---|
| **Charger** | *Chassi construído para impacto: placas espessas, servos de força e nenhuma delicadeza. Onde ele pisa, o chão cede antes dele.* |
| **Vitaspark** | *Chassi de velocidade extrema: quadris leves, dissipadores nas costas e a sensação permanente de que ele já foi embora.* |
| **Aetheric** | *Estrutura esguia ligada a fontes de energia cósmica e quântica. Ele não manipula Éter — conversa com ele.* |
| **Techno** | *Chassi de núcleo cerebral voltado à análise. Ele lê o mundo como dados — inclusive você.* |
| **Droneframe** | *Unidades pequenas e ágeis, feitas para espionagem. Difíceis de ver e fáceis de subestimar.* |
| **Forgekin** | *Tanques vivos: braços hidráulicos, blindagem fundida e a calma de quem já aguentou coisa pior.* |
| **Ghostnet** | *Corpo semi-óptico, feito de camuflagem e paciência. Se você o viu, ele deixou.* |
| **Vox** | *Chassi de aparência e emoções humanas, especializado em influência. A voz dele é a arma.* |
| **Overcore** | *Núcleos sobrecarregados em um chassi instável. Ele é mais forte do que deveria — e sabe disso.* |
| **Cryonix** | *Núcleo criogênico, resistente ao superaquecimento. Tudo o que ele toca desacelera.* |

**Vantagens (1 principal + 1 secundária por chassi)** — balanceadas em **3 UV** (Unidade de Vantagem: 1 UV ≈ +1d4 de dano
condicional, ou +1 CA condicional, ou 1 uso limitado de ferramenta exclusiva por combate). Tabela completa: **anexo `Docs/Modelos_10.md`**.

**Dois efeitos estruturais que valem saber:**
- **Forgekin**: o **−1 DEX paga o bônus de armadura** (−1 DES = −1 CA), então o "+2 → +5 de CA" é **+1 → +4 líquido**.
- **Overcore**: **SAB −1** faz dele o chassi **mais vulnerável à puxada** do Véu Profundo. O defeito é um destino.

---

## 5. CLASSES E SUBCLASSES

**6 classes base** (do DataTable `DT_PloidrekClasses`) × **3 subclasses** = 18 especializações, ortogonais aos 10 chassis.

| Classe | Atributos | Papel | Verbo central | Subclasses |
|---|---|---|---|---|
| **Blaster** | DEX/INT | DPS à distância | tiro carregado penetrante | Sniper · Carregador · Ricochete |
| **Blade** | STR/DEX | DPS corpo a corpo | combo rápido + sangramento | Duelista · Espectral · Sabotador |
| **Breaker** | CON/STR | Tank | provocação + escudo reativo | Juggernaut · Warlord · **Bulker** |
| **Support** | WIS/INT | Suporte | zona de recuperação | Luminary · Árbitro · Inquisidor |
| **Engineer** | INT/DEX | Controle | drone · torre · armadilha | **Machinist** · Fortificação · Alquimista |
| **Sentinel** | CON/WIS | Híbrido defensivo | muralha de energia | Muralha · Interceptador · Reforço de Núcleo |

> **Correção de hierarquia:** no GDD v2, *Machinist* era a classe e *Engineer* subclasse — **invertido**. Aqui vale:
> **Engineer é classe base; Machinist é subclasse.**

---

## 6. COMBATE — A ENGINE NEXUS

### 6.1 Janela de Rodada
**Rodada = 6 s**, com **tick mestre de 0,5 s**. A economia de ação é a do 5e, resolvida **em tempo real**:

| Slot por rodada | Regra |
|---|---|
| **1 Ação Padrão** | atacar · usar item · usar habilidade |
| **1 Ação Bônus** | dash · item rápido · passiva ativa · **ou um ataque extra** |
| **1 Reação** | pré-selecionada; dispara automática no **tell** (~0,3 s) |
| **Movimento** | livre (1 uso de mobilidade por rodada) |

- **Ataques** — a Ação de Ataque concede **1 a 4 ataques** (por nível), com **cadência de 1 s** entre eles.
- **Trava por categoria** — usada uma ação de uma categoria, outra do mesmo tipo fica indisponível até o fim da rodada.
- **CDs** — contam em segundos ou em rodadas (ex.: 2R = 12 s).
- **Relógio por entidade** — cada entidade tem o **próprio** relógio. Quem entra no combate depois **começa o seu**. Não há
  ordem de iniciativa nem relógio global.
- **Durações** — sempre ancoradas na **entidade afetada**, nunca em quem aplicou.

### 6.2 Resolução (matemática do 5e, sempre)

| Etapa | Quem decide |
|---|---|
| Alcance, linha de visão, área atingida | **geometria** (posição, mira, cobertura) |
| Acertou / errou / crítico | **d20 + proficiência + modificador vs CA** |
| Vantagem / desvantagem | 2d20, pega o maior/menor (terreno alto dá vantagem à distância) |
| Crítico | d20 ≥ 20 (**19–20** com Precision Module ou vantagem) |
| Controle e área | **salvaguarda**: d20 + mod + proficiência (se treinado) vs **CD = 8 + prof + mod** |

**A reação é a segunda chance defensiva e a encenação do erro.** Esquiva, bloqueio, aparo, redução de dano e absorção são
reações (é o *Shield* do 5e: reação, +5 CA). Quando a reação converte acerto em erro, **a animação é a da reação** — nunca
um "nada aconteceu". **Todo erro tem causa visível.** Taxa de acerto esperada: ~55–65 %.

### 6.3 Recursos e recuperação
- **Células de Éter**: 100, regeneração +5/2 s, custo 10–40.
- **Auto-reparo universal (fora de combate)** — *toggle*: ligado, o Runner **anda mais devagar**; carrega e dispara após
  **10 min**; desligar interrompe; **zerar o Éter desliga sozinho**; entrar em combate cancela. Custo: **1/3 do Éter máximo**;
  cura = `1d12 × ⌊Éter atual ÷ 10⌋`; sem Éter suficiente, **gasta o que tem, zera e cura esse valor sem multiplicador**.
  *Cura aqui é vulnerabilidade* — é o preço.
- **Habilidade de chassi (Aetheric)** — em combate: **2 usos por combate**, cura ao **toque** (1 quadrado = 1,52 m) em si ou
  aliado adjacente, mesma fórmula; recarrega com **descanso curto** fora de combate.

### 6.4 Mobilidade e verticalidade
| Movimento | Regra | Desbloqueio |
|---|---|---|
| **Dash direcional para o cursor** | ~3 m em 0,5 s | inicial |
| **Wall slide** | desce devagar encostado na parede | inicial |
| **Wall jump** | salta da parede na direção do input; **encadeável** | **Data Core** |
| **Pulo duplo** | 1 uso aéreo | **Data Core** |
| **Grapple** | 9 m (6 quadrados) | **Data Core** |

Cada movimento consome o **slot de Mobilidade (1 por rodada)**. **Voo está fora de escopo.** A verticalidade paga em
**Vantagem** (2d20) — a moeda do 5e — e não em stat nova. O mouse mira; o **dash vai para o cursor** e o **wall jump usa a
direção do input** (o mouse nunca disputa com o movimento).

### 6.5 Status
Gerenciador **server-authoritative**, duração ancorada na vítima, **teto de 12 status ativos** por alvo, mesmo tipo não
acumula (renova ou replica) e **retornos decrescentes** para controle: o 2º CC dura menos, o 3º é ignorado por X s.

### 6.6 Vocabulário de tempo

| Termo | Valor |
|---|---|
| Tick mestre | 0,5 s |
| **Rodada** | **6 s** |
| **Turno** | **10 rodadas = 60 s** (o "minuto" do 5e) |
| **Descanso curto** | **10 min** fora de combate |
| **Descanso longo** | **20 min** |
| Vitaciclo | 100 anos humanos (ver §2.4) |

---

## 7. INIMIGOS, LORDES E DATA CORES

### 7.1 Regra-mãe
**Simetria de matemática, assimetria de construção.** O inimigo usa **as mesmas regras de resolução** (CA, salvaguardas,
proficiência, janela, 1 reação) mas **não tem classe, nível de classe nem nós de talento** — usa **stat block**, como
monstros no 5e. Isso evita explosão de conteúdo (senão seriam 18 subclasses × 10 chassis) e mantém o custo de CPU sob
controle. **Chefes são a exceção legítima:** slots extras (Ações Lendárias) e Resistência Lendária — o chefe é um "jogador
com slots a mais", não um "jogador com classe".

### 7.2 Taxonomia e orçamento

| Categoria | Ações/rodada | Reações | Lendárias | Onde |
|---|---|---|---|---|
| **Minion** | 1 ataque simples | 0 | — | hordas, corredores |
| **Regular** | 1 (+1 bônus) | 0 | — | setores, patrulhas |
| **Elite** | 1 + 1 bônus | 1 | — | eventos, guardas |
| **Chefe de Setor** | 2 + 1 bônus | 1 | 2 + 1 resistência | instâncias / dungeons abertas |
| **Lorde de Setor** | 2 + 1 bônus | 1 | 3 + 2–3 resistências + fases | caçada semanal, mundo aberto |

### 7.3 Agressão, ameaça e entrada em combate
- **Entrar em combate = ser percebido** (área de ameaça + cone de visão + audição). Cada NPC inicia **o próprio relógio**.
- **Agressão social:** aliados propagam o combate num raio de ~15 m com 0,5–1 s de atraso, com **teto anti-"train"**
  (máx. 3 grupos / 12 inimigos) e **leash** (desengaja e reseta fora da âncora).
- **IA é "burra" de propósito:** máquina de estados + prioridade fixa (ameaça → alcance → papel). Nunca onisciente.
- **Ameaça:** dano 1:1 · cura 50 % · taunt fixa 3 s · decaimento 5 %/s · **histerese de 15 %** para trocar de alvo.

### 7.4 Legibilidade
No máximo **2 tells simultâneos** por cluster de 10 m; prioridade por ameaça; 1 cor + 1 forma por tipo de dano; áudio manda
quando a origem está fora de tela; e **token de ataque** (máx. N inimigos pressionando o mesmo alvo no melee).

### 7.5 Data Cores e Protocolos Clyffe
- **Data Core** — fragmento de N.E.R.V. extraído de chefe. É progressão **horizontal**: destrava **acesso**
  (traversal, receitas, nós, passivas) e alimenta a **Impressão de Protocolo**. Duplicatas viram **Sintonização**.
- **Protocolos Clyffe** — Data Cores **lendários**: fragmentos do modelo extinto que deixam um Runner **emular partes** do
  que os Clyffen eram. É o endgame narrativo: **não se revive a espécie, usa-se o que sobrou dela**.

### 7.6 Os Clyffen — o "Tarrasque" do jogo
| Regra | Detalhe |
|---|---|
| **Nunca é obrigatório** | nenhuma quest, rota ou Data Core depende de matá-lo |
| **Não é balanceado** | nenhuma build, nível ou grupo o torna uma luta justa — e é intencional |
| **Nunca aparece em cima do jogador** | sempre há aviso antes (áudio distorcido, o Véu se fecha) |
| **Condição de fuga explícita** | sair do Véu Profundo, quebrar linha de visão, ganhar distância |
| **Nunca é farmável** | sem respawn garantido, sem loot — o prêmio é **voltar vivo** |
| **Prêmio narrativo** | relato, marca permanente e, às vezes, um **fragmento (Protocolo Clyffe)** |

*Se você ver um deles pela névoa — corra.*

### 7.7 Orçamentos técnicos e QC
Significância por faixa (10 Hz em combate → 0,5 Hz a 40–80 m → não simula além), Significance Manager + Animation Budget
Allocator + MassEntity, replicação por relevância. **Alvos de QC:** TTK 3–5 s (minion) · 2–3 rodadas (regular) ·
6–10 rodadas (elite) · 3–6 min (chefe) · 8–15 min (Lorde); tell ≥ 0,3 s (0,8 s em ataque pesado); IA ≤ 2 ms/tick por shard
com 10 inimigos ativos. **Automação obrigatória** — a matriz categoria × nível roda em script e falha o gate fora da faixa.
Detalhe completo: `Docs/Bestiario_Lordes_DataCores.md`.

---

## 8. MODOS DE JOGO

| Modo | Descrição | Duração |
|---|---|---|
| **A. A Margem (mundo aberto)** | hub persistente: comércio, grupos, zonas seguras e contestadas, eventos mundiais | sem timer |
| **B. Instâncias de Extração** | 1–3 Runners buscam Núcleos de Éter sob a névoa; PvE e PvP (morte = perde gear carregada) | 20–40 min |
| **C. Eventos de Crise (dungeons)** | invocados com **Catalisador de Crise**; 3–4 encontros até um Lorde corrompido; Normal → Nightmare | 30–60 min |
| **D. A Caçada (guilda semanal)** | o **Lorde de Setor** surge em local secreto e precisa ser **descoberto** e invocado — evento público e disputado | semanal |
| **E. Solo Challenges** | versões solo de elites e Lordes; **modo Ironman** (uma vida) | 10–20 min |
| **F. Campos de Batalha** | 5v5 / 10v10; Choque de Éter · Dominação · Aniquilação; stats normalizados | 15–30 min |
| **G. Sazonais** | A Maré de Névoa (expansão do Véu), Guerras de Facção, Os Trials do Protocolo | mensal/semanal |

---

## 9. ITENS, CRAFTING E ECONOMIA

### 9.1 Requisitos de equipamento
**Atributos mínimos** · **classe/subclasse** · **talentos** · **proficiência** · **renome de facção** · **nível de artesão** do criador.

### 9.2 Gear
Tiers **Comum → Incomum → Raro → Épico → Lendário → Mítico** · **afixos** aleatórios · **Runewords** (runas de Éter em
gear com soquete) · **Itens de Lorde** (afixos e aparência únicos).

### 9.3 Moedas
**Créditos** (vendedores/missões) · **Fragmentos de Éter** (extração e tier alto) · **Sucata** (desmantelamento).

### 9.4 Crafting (pilar completo, não secundário)
- **Especializações:** Armeiro · Armadureiro · Implantologista · Alquimista · Runologista · Engenheiro — progressão **1–20** cada.
- **Oficinas** upgradeáveis · **contratos** de jogadores, guildas e facções · **receitas secretas** por exploração, renome
  ou decodificação · **reputação de artesão** separada.
- **Técnicas profundas:** Modificação (rerolar afixos) · Infusão (upgrade de tier) · Impressão de Protocolo · Runecrafting ·
  Desmantelamento.

### 9.5 Comércio
**O Bazar** (casa de leilão dos jogadores na Margem) · troca direta com inspeção · **sem vinculação por coleta** na maioria
do gear · contratos de crafting.

---

## 10. LEADERBOARDS E TEMPORADAS
**Rei da Extração · Caçador de Lordes · Speedster de Crise · Campeão de Campo de Batalha · Sobrevivente Ironman ·
Andarilho da Névoa · Mestre Artesão · Magnata do Mercado** — mais duas que a lore pede:
**Último a Voltar** (maior tempo sobrevivido no Véu Profundo) e **Intacto** (maior sequência sem subir o Medidor de Fadenclyffe).
**Temporadas de 3 meses** com novo Setor, novo Lorde, novos materiais e balanceamento; personagens sazonais opcionais;
passe de batalha **cosmético**.

---

## 11. ARTE E ÁUDIO

- **Estilo visual canônico (✅ decidido — manter):** sci-fi de ação de **alto contraste**, com **paleta saturada**,
  **silhuetas grandes e legíveis** e **megaestrutura industrial** — a mesma linguagem visual que originou o projeto.
  Funde **latão, cobre e ferro forjado** com **neon funcional** (azul, verde, laranja) sob um **céu dusk**.
  Paleta completa: `Docs/SteampunkPalette.md`. Descrição de mundo: `Docs/WorldBible_Nexus7.md`.
  ⚠️ **Regra de IP:** o material de referência que inspirou essa linguagem fica **isolado em `Art/reference/`** como
  estudo interno. **Ele nunca é citado** no jogo, no GDD ou em apresentação pública, e seus derivados nunca entram no build.
- **O Faden é personagem, não efeito:** névoa volumétrica que reage a luz e movimento, degrada minimapa e mente no áudio.
- **UI diegética:** HUD projetado de dispositivos de pulso, minimapa como drone holográfico. O **relógio de rodada (0–6 s)**
  precisa de spec própria — é HUD incomum e o maior risco de onboarding.
- **Áudio como mecânica:** o Véu abafa o distante e amplifica a ameaça próxima; cada Protocolo tem assinatura sonora;
  trilha synthwave-industrial que intensifica em combate e Mares de Névoa. **Tell fora de tela é áudio.**

---

## 12. ARQUITETURA TÉCNICA

- **Engine:** Unreal **5.8** neste repositório. O PloidrekRPG (5.5) é **fonte somente-leitura**: login, conta, criação de
  personagem, DataTables, mapas e config são **copiados** para cá (ver `Docs/BACKLOG_Tecnico.md`, seção Migração).
- **Networking:** mundo aberto em **shards dedicados** (100+); modos de sessão em servidores instanciados; **predição
  client-side com reconciliação**; **visão server-authoritative** (o cliente só recebe o que pode ver).
- **Anti-cheat:** validação server-side de acerto, loot e progressão; **a própria névoa limita informação** — wallhack é
  difícil por design. Rolagens de dado são **server-authoritative**.
- **Multi-hardware (restrição desde já):** PC → **mobile** → **web**. Sem Lumen/Nanite na web; orçamento de atores animados
  apertado no mobile; **input KBM + gamepad + toque desde o dia 1**. **Nada que não rode no alvo mais fraco pode ser
  dependência de gameplay** — efeito bonito pode faltar; regra de combate, não.
- **Orçamentos:** World Partition + HLOD + Data Layers por setor; IA com significância; replicação por relevância;
  Animation Budget Allocator; MassEntity para multidões.
- **Balanceamento:** **simulador obrigatório** (matriz categoria × nível × nº de inimigos) medindo TTK/DPS — sem ele, o
  balanceamento é opinião.

---

## 13. MONETIZAÇÃO
Free-to-play ético: acesso completo ao jogo, classes e modos · premium **só cosmético** · conveniência não-P2W (abas,
slots, boosters apenas para alts) · passe de batalha ~US$ 10/temporada · **sem loot boxes**.

---

## 14. ROADMAP

| Fase | Conteúdo |
|---|---|
| **1 — Fundação (meses 1–12)** | **PROTOCOL ZERO** (a campanha da release): Engine Nexus (rodada, resolução, status) · 6 classes e subclasses · capítulo I (níveis 1–20) · **Nexus-7 / Kardys** (1 setor contínuo de A Margem) · 5 Eventos de Crise · crafting básico · **MVP do Medidor de Fadenclyffe** · **arco de lore completo, terminando na revelação** (ver `Campanha_Protocol_Zero.md`) |
| **2 — Expansão (13–24)** | Extração (PvE + PvP) · 2 Campos de Batalha · A Caçada semanal · crafting avançado e economia · Temporada 1 · **Protocolos Clyffe** |
| **3 — Evolução (25+)** | capítulos II e III (21–60) · **novos mundos** (cada um com cidade e Setores próprios) · guerras de guilda e território · montarias e veículos · raids 8 jogadores · conteúdo de usuário · consoles |

---

## 15. DECISÕES ABERTAS

| # | Item | Impacto |
|---|---|---|
| 1 | ✅ **VoltStriker resolvido:** foi **teste de pipeline de IA**, sem relação com o design. A **malha 3D é descartável**; o **scaffold GAS em C++ é reaproveitável** (ver `Docs/_arquivo/VoltStriker_Inventario.md`). **Decisão aberta:** reaproveitar esse scaffold renomeado, ou reescrever do zero? | Economia grande de tempo: o kit do Blaster já existe em código |
| 2 | ✅ **Estilo visual resolvido:** **mantém-se** a linguagem original do projeto (referência de estudo isolada em `Art/`, nunca citada) | Direção de arte, materiais, PCG |
| 3 | **Morte, downtime e respawn** — HP entre combates, poções, penalidade de morte, reviver aliado | **Adiado por decisão** até o MVP estar redondo; entra antes do PvP |
| 4 | **HUD do relógio de rodada** (0–6 s) — spec de UI e onboarding | Maior risco de usabilidade |
| 5 | **Números da regra de alcance** normal/longo (a desvantagem por distância já existe) | Balanceamento à distância |
| 6 | **Sistema de carga/peso** — substituído por "Peso Morto" no Forgekin; decidir se carga existe | Inventário e economia |
| 7 | **Lista de "testes" fora de combate** (o trait social dá +1d4 em testes) | Amarra com proficiências |
| 8 | **Regras de armadura** (limite de DES, proficiência, penalidade) | CA e build |
| 9 | ✅ **Resolvido: a cidade se chama `Kardys`** — fusão de **Kardashev + Dyson** (`Kardyshev`/`Kardyson`), encurtada na fala comum e herdada dos registros humanos | Lore, mapas, assets, UI de mapa |
| 10 | **Como se viaja entre mundos?** Estação, portão, torre ou nave — e o que disso sobreviveu ao Faden | Estrutura de conteúdo e expansão |
| 11 | **O que é "1 ciclo" no lote de Runners?** Recomendação: **ciclo de produção** (jargão da instalação, sem equivalência fixa com vitaciclo) | Evita colidir com a escala de 100 mil anos |
| 12 | **Nome da ala/instalação** onde o jogador acorda — precisa soar como hospital e ficar sinistro quando relido | Cenário, diálogo, missão |
| 13 | ✅ **Resolvido:** **a névoa completou o ciclo dela ali** — Kardys é um **epicentro**, não um lugar limpo | Base de toda a campanha |

### 15.1 Kardys — nome fechado, e por que ele é melhor que os candidatos

| Candidato (descartado) | O que perdia |
|---|---|
| **Arco Zero** | bom nome, mas "zero" só ganhava sentido na revelação |
| **Vértebra** | poético, mas é **descrição** — e descrição não conta história |
| **A Forja** | confundia com o nome do universo (*Aether Forge*) |
| **Cintila** | neutro: não dizia nada de errado nem nada de certo |

**Kardys** venceu porque é o único **fóssil linguístico** do conjunto:

- É a fusão de **Kardashev + Dyson** (`Kardyshev` / `Kardyson`) — a **escala de civilização** dos humanos extintos.
- Chegou até os Runners por **erosão da fala**: os Arquitetos leram o registro, adotaram a palavra e **nunca souberam o que ela mede**.
- O resultado é a ironia inteira do mundo em **duas sílabas**: a cidade carrega o nome do **grau máximo de civilização** — e é um
  **epicentro em ruínas** onde os Runners são fabricados por um processo que ninguém entende.

> **Regra de nomenclatura confirmada de novo (4ª vez):** o **registro oficial** guarda a palavra antiga e erudita
> (`Kardyshev`, `ACE`), e o **registro popular** encurta até virar outra coisa (`Kardys`, `o Faden`).
> O jogador que presta atenção percebe que **o nome das coisas é sempre mais velho que quem as usa**.

---

## 16. ANEXOS E HISTÓRICO

| Documento | Papel |
|---|---|
| `Docs/Modelos_10.md` | **anexo canônico** dos 10 chassis: bônus, defeitos, vantagens, balanceamento em UV |
| `Docs/Bestiario_Lordes_DataCores.md` | **anexo canônico** de inimigos: taxonomia, orçamentos, agressão, tells, QC |
| `Docs/GDD_v3_Fundacao.md` | registro histórico das decisões (de onde cada regra veio) |
| `Docs/BACKLOG_Tecnico.md` | issues técnicas e de conteúdo, com critério de aceite |
| `Docs/Campanha_Protocol_Zero.md` | **material restrito** — o arco narrativo da release, com as revelações e as pistas a plantar |
| `Docs/SteampunkPalette.md` · `Docs/WorldBible_Nexus7.md` | arte: paleta e bíblia visual |
| `Docs/deep-research-report.md` · `Docs/ImportPipeline.md` · `Docs/AssetPipeline3D.md` | pipeline técnico (mesh, import, IA→UE) |
| `Docs/ENGINEERING_PLAYBOOK_UNREAL.md` | regras de engenharia, boundaries e DoD |
| GDD v1 · v2 · bíblia visual antiga · "documento consolidado de combate" | **superados e arquivados** em `Docs/_arquivo/` (com cópias em `Art/_arquivo/`) — mantidos só como registro de evolução |
| `Docs/_arquivo/VoltStriker_Inventario.md` | inventário do protótipo (o que é descartável e o que é reaproveitável) |
