# Bestiário, Lordes de Setor e Data Cores — Como Construir

> **Escopo:** (1) regra-mãe dos inimigos, (2) taxonomia e orçamento de janela, (3) orçamento de IA/performance,
> (4) legibilidade (tells e tokens de ataque), (5) Data Cores, (6) pipeline de produção data-driven, (7) QC numérico.
> **Base:** práticas verificáveis (documentação da engine + SRD 5.1 + princípios públicos de *readability*) adaptadas
> ao sistema de **Janela de Rodada (6 s / tick 0,5 s)**.
> **Contexto de projeto:** o projeto canônico é este repositório **UE 5.8**, que herda conteúdo do PloidrekRPG (UE 5.5,
> **somente leitura**).
> **Nomenclatura (ver `Docs/PloidrekRPG_GDD_v3.md`):** **Runners** (o povo) · **N.E.R.V.** (o núcleo) · **Fadenclyffe / o
> Faden** (a névoa) · **Apagados** (os tomados) · **Retornados** (quem voltou) · **Clyffen** (o modelo extinto).

---

## 0. Status

- ✅ **Decidido:** inimigos seguem as **mesmas regras de resolução** do jogador (mesma matemática: CA, salvaguardas,
  proficiência, janela de rodada, 1 reação, CD por categoria).
- 🟡 **Decidido com ressalva:** essa simetria é de *matemática*, **não** de *construção* — ver §1 (é o ponto que evita
  explosão de conteúdo e afogamento de CPU).
- ✅ **Nomenclatura fechada:** **Apagados** é o nome canônico dos tomados (não usar "Estáticos", "Radiados" etc.).
  Termos de IP ("Reploid", "Maverick") estão banidos de dados e documentos — ver `IP-001` e `MOD-001` no backlog.

---

## 1. Regra-mãe: simetria de matemática, assimetria de construção

| Dimensão | SIMÉTRICO (inimigo = jogador) | ASSIMÉTRICO (inimigo ≠ jogador) |
|---|---|---|
| Resolução | mesma CA, salvaguardas, proficiência, vantagem/desvantagem | — |
| Tempo | mesma janela de 6 s, mesmo tick de 0,5 s | — |
| Economia de ação | 1 ação + 1 bônus + 1 reação por rodada | categoria de ações pode ser reduzida (minion: só ataque) |
| Construção | — | **stat block**, não classe: sem nível de classe, sem nós de talento, sem subclasse |
| Kit | — | 1–3 habilidades + 1 reação, no máximo |
| Decisão | — | qualidade/cadência limitada por perfil de IA e orçamento (§3) |
| Progressão | — | CR/nível de ameaça, não XP de classe |

**Por que:** o D&D 5e nunca deu níveis de classe a monstros — monstros usam **stat block + Challenge Rating**. Aplicar
isso é o que mantém o custo sob controle: se inimigo usasse a árvore de jogador, seriam **18 subclasses × 10 modelos**
de conteúdo para balancear. Estatística igual, construção diferente.

**Exceção legítima (e ela já existe no 5e):** chefes. No SRD, **Ações Lendárias** são ações fora do próprio turno e
**Resistência Lendária** anula uma salvaguarda falha — isso mapeia 1:1 no nosso modelo como *slots extras por janela*.
Ou seja: chefe = jogador "com slots a mais", não jogador "com classe".

> Fontes: SRD 5.1 (monstros, CR, Ações Lendárias/Lair Actions, Resistência Lendária) — disponível via open5e.

---

## 2. Taxonomia e orçamento de janela

| Categoria | Papel no jogo | Ações/rodada | Bônus | Reações | Lendárias | Onde aparece |
|---|---|---|---|---|---|---|
| **Minion** (enxame) | pressão, custo ínfimo | 1 ataque simples | 0 | 0 | — | corredores, hordas, eventos |
| **Regular** | tropa padrão | 1 | 0–1 | 0 | — | setores, patrulhas |
| **Elite** | mini-chefe | 1 | 1 | 1 | — | eventos, guardas de gate |
| **Chefe de Setor** | marco de progressão | 2 | 1 | 1 | 2 + 1 resistência | instância / dungeon aberta |
| **Lorde de Setor** | caçada semanal (evento público) | 2 | 1 | 1 | 3 + 2–3 resistências + fases | mundo aberto |

Regras de contorno:
- **Nada de kit de classe em inimigo.** Se um inimigo precisa de 5 habilidades, ele é um Chefe — não um Regular "rico".
- **Reação é o item mais caro do jogo em percepção.** Minion com reação (esquiva/parry) é o caminho mais rápido para o
  jogador sentir que "errou sem ter errado" — ver §4.
- **Cadeia de fraqueza** (do GDD) é atributo do Lorde, não do inimigo comum: fraquezas elementais cruzadas entre Lordes.

---

## 3. Orçamento de IA — o que impede o servidor de morrer

O custo não é "quantos inimigos existem", é **quantos inimigos simulam por tick**. Escalonar por significância:

| Faixa | Simulação | Tick | Reações |
|---|---|---|---|
| ≤ 15 m (combate ativo) | IA completa, colisão precisa, GAS ativo, tells completos | 10 Hz | sim |
| 15–40 m | navegação simplificada, sem percepção fina nem EQS | 2–5 Hz | não |
| 40–80 m | agregado por grupo, sem animação individual | 0,5 Hz | não |
| > 80 m / fora de relevância | não simula (estado persistido em dados) | — | não |

Ferramentas da engine a usar (existem e são documentadas pela Epic):
- **Significance Manager** — permite definir uma função de significância e escalar tick/LOD por objeto. É exatamente o
  gancho para a tabela acima.
- **Animation Budget Allocator** — limita quantos skeletal meshes recebem atualização de animação completa por frame;
  é a defesa contra "50 inimigos animados = 12 fps".
- **MassEntity / MassAI** — framework data-oriented para multidões (há relatos públicos de 10k NPCs). Candidato para
  enxames e NPCs de cidade; **verificar suporte no servidor dedicado da versão usada antes de adotar**.
- **Replication Graph + relevância** (`NetCullDistance`, interest management) — premissa já registrada no playbook:
  o cliente só recebe o que importa.

**Alvo inicial (medido, não estimado):** IA ≤ 2 ms/tick por shard em combate com 10 inimigos ativos; ≤ 0,5 ms/tick por
inimigo adicional. Sem medição, qualquer número é fé. → BACKLOG `PERF-001`, `PERF-002`.

---

## 4. Legibilidade: tells, tokens de ataque e anti-frustração

Em D&D os turnos **serializam** as ações. Em tempo real com 6 s e vários inimigos, o problema nº 1 deixa de ser dano e
passa a ser **colisão de tells**. Sem regra explícita, o combate vira ruído.

Regras propostas:
1. **Orçamento de tells:** no máximo **2 tells simultâneos** por cluster de 10 m; o restante entra em fila curta.
2. **Prioridade por ameaça:** dano potencial × tempo até o impacto decide quem "ganha" o tell.
3. **Gramática visual fixa:** 1 cor + 1 forma por categoria de ataque (perfurante / cortante / esmagamento / energia).
   Nunca reutilizar a mesma cor para "ataque" e "cura".
4. **Áudio > visual fora de tela:** o GDD já define áudio como mecânica; tell de origem fora do cone de visão **tem** de
   ser audível e distinto.
5. **Token de ataque (prática consolidada em jogos de melee em grupo):** no máximo **N atacantes corpo a corpo**
   pressionando o mesmo alvo; os demais circulam/esperam. Sem isso, 5 inimigos batendo junto mata sem chance de leitura.
6. **Anti-frustração (fairness):** inimigo só reage ao que **percebe** (sem informação onisciente), com erro/atraso
   calibrado, e no máximo 1 reação por rodada. Se seis inimigos reagem ao mesmo skill shot, o jogador aprende que
   acertar não importa.

Referência de leitura: princípios públicos de *readability* em ARPG (Game Developer, "Designing for Difficulty:
Readability in ARPGs") e o debate público sobre "inimigos esperando a vez para atacar" em jogos de ação em grupo.

---

## 5. Data Cores

**O que é:** fragmento de **N.E.R.V.** extraído de um chefe. É progressão **horizontal** — dá *acesso*, nunca poder
direto (coerente com a decisão de teto de nível: diferença entre jogadores vem de acesso).

Dois usos, sempre:
1. **Destravar rota/traversal** — gate diegético no mundo contínuo (a "dungeon aberta").
2. **Impressão de Protocolo** — imprimir em gear (ver GDD §9).

Duplicatas nunca viram lixo: entram em **Sintonização** (reforço de upgrade) — evita economia morta.

Schema proposto — `STR_DataCore`:

| Campo | Tipo | Nota |
|---|---|---|
| `CoreId` | Name | chave **imutável** (não usar display name como chave) |
| `DisplayName` | FText | localizável |
| `Tier` | Enum | Comum → Incomum → Raro → Prime |
| `SourceBossId` | Name | 1 core por chefe único |
| `UnlockType` | Enum | Traversal \| Recipe \| Node \| Passive |
| `UnlockPayloadId` | Name | aponta para rota/receita/nó |
| `ResonanceValue` | Int | valor de Sintonização em duplicata |
| `bTradeable` | Bool | **proposta: false na v1** (evita RMT e colapso de preço) |

---

## 6. Pipeline de produção de um inimigo (o "como construir")

1. **Escolher a categoria** (§2) — ela define o orçamento de janela e o teto de kit.
2. **Criar a linha** em `DT_Enemies` (row struct `STR_Enemy`).
3. **Kit:** 1–3 habilidades **por ID** (`DT_Abilities`), nunca texto solto na linha.
4. **Perfil de IA:** comportamento + disciplina de reação (§4) + participação no token de ataque.
5. **Classe de significância** (§3) — define custo por faixa de distância.
6. **Telemetria:** TTK alvo, dano/rodada, tempo de tell, tells/segundo.
7. **QC** (§7) → aprovar → publicar.

Schema proposto — `STR_Enemy`:

```
EnemyId (Name, imutável) | DisplayName (FText) | Category (Enum) | Family (Tag)
Level | AttrMods {STR,DEX,CON,INT,WIS,CHA} | AC | HP
SaveProficiencies[] | Resistances[] | Vulnerabilities[]
AbilityIds[] (≤3) | ReactionId | WindowBudget
SignificanceClass | TokenRole | DropTableId | CoreId? | Tags[]
```

---

## 7. QC numérico (critérios de aceite)

| Métrica | Alvo |
|---|---|
| TTK Minion | 3–5 s (≈ 1 rodada) |
| TTK Regular | 2–3 rodadas |
| TTK Elite | 6–10 rodadas |
| TTK Chefe / Lorde | 3–6 min / 8–15 min |
| Dano do inimigo vs HP do jogador | nunca matar em < 2 rodadas sem tell longo |
| Tell para reação | ≥ 0,3 s |
| Tell de ataque pesado | ≥ 0,8 s |
| Tells simultâneos | ≤ 2 por cluster de 10 m |
| Custo de IA | ≤ 2 ms/tick por shard com 10 inimigos ativos |

**Automação:** um script roda a matriz (categoria × nível × nº de inimigos) e **falha o gate** se o TTK sair da faixa.
Sem isso, balanceamento vira opinião. → BACKLOG `PERF-005`.

---

## 8. Questões abertas

1. Inimigo tem **Células de Éter** próprias (recurso) ou usa CDs fixos por habilidade?
2. Enxames usam **MassEntity** desde o MVP ou começam com Actors + tick LOD?
3. Chefe pode usar **Efeitos Ativos de Item** (como o jogador)?
4. `Data Core` duplicado é negociável entre jogadores?
5. ✅ Resolvido: **Fadenclyffe / o Faden** (fenômeno) e **os Apagados** (tomados).
6. Ferramentas de orçamento (§3) precisam de validação **na versão de engine alvo**.

---

## 9. Agressão, ameaça e entrada em combate (o que faltava por escrito)

A Janela de Rodada é **por entidade** — logo, "quem entrou em combate e quando" deixa de ser detalhe e passa a ser
regra de jogo. Este é o sistema que responde "quem ataca quem".

### 9.1 Como um combate começa
- Cada NPC tem **área de ameaça** (raio), **cone de visão** e **linha de audição**.
- Entrar em combate = **ser percebido**: visto, ouvido, ou ter/ser alvo de um ataque.
- Ao perceber, o NPC **inicia a própria janela (T=0)**. Não existe relógio global de grupo.

### 9.2 Agressão social (o grupo entra junto)
- Aliados do mesmo grupo/squad propagam o combate em um raio curto (**~15 m**) com atraso de **0,5–1 s**.
- **Limite anti-"train":** uma propagação para imediatamente após **N grupos** (sugestão: 3) ou **X inimigos**
  (sugestão: 12). Sem esse teto, um pull errado arrasta o mapa inteiro e derruba servidor e jogador ao mesmo tempo.
- **Leash/reset:** se o NPC se afasta mais de X m da âncora ou fica Y s sem alvo válido, ele **desengaja**, reseta HP e
  volta ao posto. Evita kite infinito e vazamento de estado.

### 9.3 Tabela de ameaça (threat)
| Ação | Gera ameaça |
|---|---|
| Dano causado | 1 : 1 do dano |
| Cura / escudo em aliado | 50 % do valor |
| Provocação (taunt) | fixa o alvo por 3 s |
| Decaimento | −5 %/s fora de combate |
| Troca de alvo | **histerese**: só troca se a diferença passar de 15 % (evita flip-flop) |

### 9.4 IA "burra" — sim, e é a prática correta
Sua proposta é exatamente o padrão da indústria: **não é IA generativa nem aprendizado**. É
**máquina de estados + tabela de prioridade fixa** (ameaça → alcance → papel). Regras:
- Poucas decisões por janela; nenhuma decisão cara fora da faixa de significância (§3).
- Percepção simples (cone + raio + linha de visão), nunca onisciente.
- Papel definido no stat block (`TokenRole`): melee-token, ranged, suporte — liga direto ao token de ataque (§4).

### 9.5 Status
- Gerenciador **server-authoritative**, duração **ancorada na vítima**.
- Mesmo tipo não acumula (replica ou renova); teto de status ativos por alvo (**12**) para caber em UI e orçamento.
- Retornos decrescentes (DR) para controle: 2º CC na sequência dura menos, 3º é ignorado por X s.

### 9.6 Ordem de combate do jogador
Sua janela começa no **primeiro evento de combate**: dano causado, dano recebido ou alvo hostil percebido — o que vier
primeiro. Sair de combate = **Y s sem nenhum desses eventos** (sugestão: 6 s = 1 rodada).

---

## 10. Multi-hardware (PC → mobile → web) — restrição registrada

> Pedido: o jogo deve poder rodar em hardwares variados, incluindo **mobile** depois e **web**.
> Isso **não é uma meta futura de otimização**: é uma restrição de arquitetura que precisa valer desde agora.

**O que é "CPU por shard" (explicando sem jargão):** um servidor de MMO roda vários "mundos" (shards); cada shard
processa IA, física, status e rede de centenas de personagens a cada tick. Se cada NPC custa caro por tick, o número de
jogadores por servidor cai e o custo de infraestrutura sobe. Como o jogo também precisa rodar em celular e navegador,
o orçamento precisa ser apertado **no servidor e no cliente ao mesmo tempo**.

**Consequências diretas (a validar na versão alvo da engine):**
| Alvo | Restrição típica |
|---|---|
| Web | Sem Lumen/Nanite; orçamento de memória e download apertados; WebGPU/WebGL limitam efeitos |
| Mobile | Sem ray tracing; poucos atores animados simultâneos; UI precisa funcionar em toque |
| PC | Referência de qualidade, **não** o denominador comum |

**Regra de ouro:** nada que não rode no alvo mais fraco pode ser **dependência de gameplay**. Efeito bonito pode faltar
no celular; regra de combate não pode.

**Bônus coerente:** o `ENGINEERING_PLAYBOOK_UNREAL.md` §4 já obriga input **KBM + gamepad + toque desde o dia 1** —
é exatamente a prática que esse alvo exige. Manter.

**Encaminhamento:** `Docs/Target_Hardware.md` (BACKLOG `PROC-004`).
