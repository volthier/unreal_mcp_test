# BACKLOG Técnico — Débitos, Riscos e Ordenação

> **Como usar:** cada linha é uma **issue atômica** com critério de aceite (DoD), executável por humano **ou** agente.
> **Regra de casa:** o projeto canônico é o **PloidrekRPG** (feito à mão, sem IA). Itens que tocam assets/textos de lá
> devem ser **aplicados manualmente** por uma pessoa — este backlog é produzido no laboratório `AI_MEGA_MAN_TEST`.
> **Severidade:** 🔴 Alta (bloqueia ou contamina dado) · 🟠 Média (dívida que cresce) · 🟡 Baixa (higiene).

---

## DADOS — DataTables e schema

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `DADOS-001` | `Role`, `PrimaryAttribute`, `SecondaryAttribute` são **string** — `"DEX"` e `"dex"` viram valores diferentes | 🔴 | Colunas tipadas como **enum ou GameplayTag**; reimportado; nenhum literal de atributo no código |
| `DADOS-002` | `Feats` é **string única com bullets separados por \n** — impede i18n e impede o GAS ler os feats | 🔴 | `DT_Feats` (`FeatId`, nome, descrição, efeito, tags) + `Feats` vira **lista de IDs** |
| `DADOS-003` | Rows sem **ID estável** nem versão de schema (a chave é o display name) | 🟠 | Declarar RowName imutável + campo `SchemaVersion`; documentar regra de renomeação |
| `DADOS-004` | Texto **PT embutido no dado** (descrições, traits, features) | 🔴 | Campos de texto como **FText**; definida convenção de chave de localização; nenhum texto em código/UI hardcoded |
| `DADOS-005` | `DT_ReploidClasses.json` vive solto em `~/Documents` e em `/Users/Shared/.../UNREAL/`, além do `DT_PloidrekClasses` no projeto | 🔴 | **Uma** fonte da verdade escolhida (recomendado: export JSON/CSV versionado junto do projeto) e as cópias soltas removidas |
| `DADOS-006` | DataTable só existe como `.uasset` binário — sem diff, sem review | 🟠 | Export para **JSON/CSV versionado** no fluxo; `.uasset` passa a ser artefato derivado |
| `DADOS-007` | `STR_PloidrekModel` tem `Trait` e `Feature` sem definição escrita de diferença | 🟡 | Documentar semântica dos dois campos (ou fundir em um) |
| `DADOS-008` | Tipo de `BonusAttributes` (mapa de 6 atributos) sem regra de faixa/cap | 🟠 | Definir min/max por modelo e validar soma no editor |
| `DADOS-009` | `BonusAttributes` das 10 models não está em formato diff-ável (só `.uasset`) | 🔴 | Export CSV (`Docs/Modelos_10.md` §4) e versão em JSON no repositório |

## Arquivo morto e protótipo

> Documentado em `Docs/_arquivo/README.md` (o que foi arquivado e por quê) e
> `Docs/_arquivo/VoltStriker_Inventario.md` (inventário completo do protótipo).

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `ARQ-001` | **Decisão:** reaproveitar o **scaffold GAS** do protótipo, renomeado | 🔴 | ✅ **decidido: reaproveitar com `Runner*`** — plano completo em `Docs/Plano_Renomeacao_Runner.md` |
| `ARQ-002` | `Content/VoltStriker/` (**67 arquivos**, 7 variantes de SKM, 5+ skeletons) ocupa o Content Browser | 🟠 | Arquivado com **Fix up Redirectors** — depende de `ARQ-001` |
| `ARQ-003` | `Content/VoltStriker/Textures/` contém **PNGs 2K/4K dentro do Content** | 🟡 | PNGs movidos para `Art/`; só `.uasset` fica em `Content/` |
| `ARQ-004` | Existe **um** registro de que o pipeline IA→FBX→UE funcionou ponta a ponta | 🟡 | Guardar `VoltStriker.blend` + 1 textura 2K como prova de fluxo antes de descartar o resto |
| `ARQ-005` | `Config/DefaultEngine.ini` aponta `GlobalDefaultGameMode` para `BP_VoltStrikerGameMode` | 🟠 | Retargetado para o game mode do jogo quando ele existir |

## Modelos (chassis) — conteúdo extraído de `DT_PloidrekModels`

> Detalhe completo e a tabela reconstruída: `Docs/Modelos_10.md`

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `MOD-001` | As **10 descrições** começam com o termo de IP "Reploids" | 🔴 | ✅ **reescritas** — texto oficial em `PloidrekRPG_GDD_v3.md` §4; falta aplicar no DataTable do projeto |
| `MOD-002` | ✅`Vox` definido = linha `Synthion` (CHA +2) | ✅ | feito em `Modelos_10.md` §3 |
| `MOD-002b` | Linha `Runner` (DEX +2, **velocidade**) ainda sem nome — colisão com "Runner = o povo" | 🔴 | Nome escolhido (Vetor / Corisco / Ímpeto / Strider / Zephyr) |
| `MOD-009` | **Balancear as 2 vantagens dos 10 modelos** com potência comparável — requisito declarado | 🔴 | Proposta aplicada (`Modelos_10.md` §5) e validada no simulador |
| `MOD-003` | Aetheric *"por descanso curto"* e Ghostnet *"1x por descanso longo"* — **mecânica de descanso não existe** | 🔴 | Reescrever para 2 usos/combate + `DEC-09` (10 min) |
| `MOD-004` | Forgekin *"+5 de HP no N1 e +1 por nível"* conflita com o cálculo novo (`3 × dado + CON`) | 🔴 | Trait reescrito na nova fórmula |
| `MOD-005` | DR fixo *"reduz 3 de dano por acerto"* é forte com 4 ataques/rodada | 🟠 | Validado no simulador; ajustar valor se estourar TTK |
| `MOD-006` | Overcore *"1/4 do HP perdido"* — base ambígua (HP máximo ou dano recebido?) | 🟠 | Regra escrita |
| `MOD-007` | Ghostnet *"um dado extra de dano"* sem dado definido | 🟡 | Dado especificado (ex.: 1d6) |
| `MOD-008` | Padronizar *"turno"* → **rodada** e *"level"* → **nível** nos 20 textos | 🟡 | Textos revisados |

## i18n / Localização

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `I18N-001` | Sem convenção de chaves nem tabela de strings | 🟠 | Formato de chave definido (`UI.Classe.Blaster.Feature`) + tabela de strings criada; PT-BR como base |
| `I18N-002` | Nomes de classe/atributo misturam PT e EN (`Blaster`/`Descrição`) | 🟡 | Regra explícita: identificadores em EN, conteúdo em FText localizado |

## IP / Legal

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `IP-001` | `Docs/MegaManX_WorldBible.md` cita MMX/Capcom no próprio documento | 🔴 | ✅ **substituído por `Docs/WorldBible_Nexus7.md`** (nomenclatura nova); o arquivo antigo fica só como arquivo morto |
| `IP-002` | **26 assets** em `Content/MegamanX/` + `BP_MegaManX` em `Content/Characters/` | 🔴 | Renomeados para o roster próprio ou arquivados fora de `Content/` |
| `IP-003` | **8 materiais** `M_MMX_*` em `Content/Level/` | 🔴 | Renomeados por tema (ex.: `M_Nexus7_Sky`) |
| `IP-004` | Arquivos de estudo com nome/derivação de IP em `Art/` (`Megaman.obj`, `Megaman.stl`, `reference/Megaman.fbx`, `generated/world/{reploid,maverick}.png`, `Classes/sample_maverik.jpg`) | 🔴 | 🟡 **pasta `Art/reference/_ip/` criada** para recebê-los; mover e renomear; nenhum derivado entra no build |
| `IP-008` | **Regra de referência:** material de estudo nunca é citado no jogo, no GDD ou em apresentação | 🔴 | ✅ regra escrita em `PloidrekRPG_GDD_v3.md` §11 e `WorldBible_Nexus7.md` |
| `IP-005` | Sem **manifesto de proveniência** por asset (origem, licença, se derivou de referência IP) | 🔴 | `Art/PROVENANCE.md` com 1 linha por asset binário e regra de aceite |
| `IP-006` | Licenças de IA (Hunyuan3D, TRELLIS.2, Meshy, LLaMA-Mesh) não revisadas para uso comercial | 🟠 | Checklist de licença por ferramenta; registro de quais assets vieram de qual |
| `IP-007` | **O nome do jogo (`PloidrekRPG`) é foneticamente próximo de termo protegido de terceiros** | 🟠 | Decisão **consciente e registrada**: é o ativo mais visível do projeto. Revisão de marca recomendada **antes de qualquer anúncio público** |

## Performance / Engenharia

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `PERF-001` | Sem **orçamento medido** de IA por shard | 🔴 | Script de profiling; número publicado (ms/tick com 10 e 50 inimigos) |
| `PERF-002` | Significance Manager e Animation Budget Allocator não configurados | 🔴 | Configurados e medidos com N=50 atores animados; antes/depois registrado |
| `PERF-003` | Sem Replication Graph / gestão de relevância | 🔴 | Configurado **antes** de qualquer teste multiplayer; documentado o que replica |
| `PERF-004` | **Escala de movimento inconsistente:** D&D = 30 ft/rodada = **1,52 m/s**; o protótipo roda ~600 uu/s (4× mais rápido) | 🔴 | `Docs/Escala_Unidades.md` como fonte única (uu ↔ ft ↔ m ↔ quadrados ↔ tempo) e valores do protótipo realinhados |
| `PERF-005` | Sem **simulador de balanceamento** (TTK/DPS) — balanceamento por opinião | 🟠 | Script roda a matriz categoria × nível e falha o gate fora da faixa alvo |
| `PERF-006` | Sem orçamento de animação por ataque (cadência de 1 s entre ataques ainda não cabe em animação) | 🟠 | Documentado o split animação/recuperação para 1 s, 2 s e 3 s de cadência |

## Organização de Conteúdo

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `ORG-001` | `Content/` organizado **por IP** e não por tipo | 🟠 | Árvore alvo do `Docs/OrganizationAnalysis.md` §3 aplicada com fix-up de redirectors |
| `ORG-002` | **Binários versionados no git sem LFS** — medido: **295 MB de `.git`**, 649 arquivos versionados, e **282 MB só de binários pré-compilados do plugin** (`unreal-mcp-bridge` de 69–75 MB em 4 plataformas) | 🔴 | Ver **`ORG-010`** — estrategia de versionamento definida e aplicada |
| `ORG-010` | **Estratégia de controle de versão para projeto com binários** (decisão) | 🔴 | Regra escrita: o que entra no git (texto, código, dado, **finais aprovados**) e o que não entra (**gerado**, intermediário, fonte DCC pesada). LFS ligado; binários de plugin fora do histórico |
| `ORG-011` | **`.uasset`/`.umap` são binários e não fazem merge** — dois editores no mesmo asset corrompem | 🔴 | Regra de **lock**: só humano edita asset pelo editor; agente só toca texto, código e dado |
| `ORG-012` | Binários de plugin **no histórico** (imutável) — **288 MB** medidos no commit anterior | 🟠 | Aguardando decisão (reescrita de histórico adiada pelo autor). O envio futuro **já parou**: o plugin foi arquivado |
| `ORG-013` | ✅ **Plugin de terceiros `UnrealMCP` REMOVIDO do projeto** — 605 MB apagados do disco e **288 MB fora do versionamento**. Estava desabilitado, sem referência em `Config/` e o `.mcp.json` nunca apontou para ele; a MCP da engine 5.8 é a que roda | ✅ | Feito; `.uproject` limpo, `.gitignore` ajustado e `AssetPipeline3D.md` atualizado |
| `ORG-003` | 8 variantes de `SKM_*` e 5 skeletons — versioning fora de controle | 🟠 | **1 skeleton canônico** por categoria + 1 mesh final + LODs; o resto arquivado |
| `ORG-004` | Fontes `.fbx` dentro de `Content/` | 🟡 | Movidos para `Art/` |
| `ORG-005` | Duplicatas de GDD em `Docs/` e `Art/` (v1 e v2, idênticas) | 🟡 | ✅ **resolvido:** movidas para `Docs/_arquivo/` e `Art/_arquivo/`; `Art/` só contém arte |

## Migração — PloidrekRPG (UE 5.5, BP) → este projeto (UE 5.8)

> **Decisão fechada:** o Ploidrek antigo **nunca é escrito ou alterado** — ele é fonte de **somente leitura**.
> O destino é este projeto, que passa a ser o Ploidrek em **UE 5.8**, recebendo login/senha, criação de personagem,
> DataTables e mapas vindos de lá, e ajustado ao que está sendo decidido nos documentos de design + assets.

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `MIG-001` | Copiar (somente leitura) o conteúdo relevante do Ploidrek: `Content/Data/*`, `Content/Blueprints/{UI,Systems,Characters}`, `Content/Maps/*`, `Content/Input/*`, `Config/*` | 🔴 | Cópia abrindo no 5.8; **zero escrita** no repositório de origem |
| `MIG-002` | Validar plugins que podem não existir na mesma versão em 5.8 (CommonUI, OnlineSubsystemEOS, OnlineServicesOSSAdapter) | 🔴 | ⏳ **parcial:** `OnlineSubsystemEOS` + `OnlineSubsystemUtils` + `OnlineServicesOSSAdapter` **existem no 5.8 e estão habilitados**; login EOS migrado e compilando (`URunnerSession::LoginWithEOS`) — falta a **execução** com credencial real. `CommonUI` segue não avaliado |
| `MIG-003` | Portar `Docs/ENGINEERING_PLAYBOOK_UNREAL.md` para cá e corrigir a versão citada (5.7 → 5.8) | 🟠 | Playbook vigente neste repositório |
| `MIG-004` | Definir o destino do repositório antigo | 🟠 | Decisão escrita: congelado e somente leitura, mantido como histórico |
| `MIG-005` | Reescrever os DataTables migrados no schema corrigido (`DADOS-001/002/004`) | 🔴 | DataTables no schema novo, validados no editor |
| `MIG-006` | Trazer `DT_PloidrekModels` completo (10 modelos) e exportar cópia JSON/CSV versionada | 🟠 | ✅ **resolvido:** `Data/DT_Chassis.csv` + `Data/DT_Classes.csv` versionados, importados para `Content/Data/*.uasset`; 11 chassis (10 + Clyffen extinto) e 6 classes |
| `MIG-007` | **Segredo do EOS fora do Git** — o canônico traz o `ClientSecret` no `DefaultEngine.ini` versionado | 🟡 | ✅ o segredo é **obrigatório** (o `EOS_Platform_Create` valida ClientCredentials completas: sem ele o log diz `ClientSecret cannot be null` e o EOS não sobe) e mora em `Config/<Plataforma>/<Plataforma>Engine.ini`, gerado por `Tools/unreal/setup_eos_secret.py` e coberto pelo `.gitignore`; o arquivo versionado guarda só os identificadores públicos |
| `MIG-010` | **Conta local não verifica nada** — o e-mail é só uma chave, a senha usa MD5 e não há recuperação | 🟡 | Em DEV está ok (o EOS/EAS é quem valida identidade); falta decisão escrita de quando a conta local sai do build |
| `MIG-009` | **Segredo de cliente no build final** — o EAS no modelo "cliente confidencial" exige o ClientSecret **no cliente**, que é o oposto do que se quer num build distribuído | 🔴 | Decisão escrita: ou o build público usa **EOS Connect** (cliente público, token vindo de backend), ou o login do build distribuído passa por um serviço nosso. Em DEV fica como está |
| `MIG-008` | **Servidor autoritativo** para conta e personagens — hoje a lista de Runners é lida/gravada no cliente (`Saved/RunnerCharacters.tsv`) | 🔴 | Salve/carregue via backend (EOS Player Data Storage ou serviço próprio); o cliente nunca é a fonte da verdade |

## Processo / CI

| ID | Issue | Sev | DoD |
|---|---|---|---|
| `PROC-001` | **Agentes e subagentes são esperados** neste pipeline — o playbook §0 e §17 já definem como (MUST / MUST NOT) | 🔴 | Playbook vigente aqui e citado no onboarding; nenhuma tarefa delegada sem DoD |
| `PROC-002` | Sem CI (build + smoke test) apesar de o playbook exigir como gate | 🟠 | Pipeline mínimo: build + smoke (abre mapa, spawna, 1 inimigo, ataque funciona) |
| `PROC-003` | Playbook cita UE 5.7 e o projeto alvo é 5.8 | 🟡 | Versão corrigida no playbook |
| `PROC-005` | **Custo do véu e da vitrine em hardware fraco** — o véu desenha ~20 quads por quadro no Slate e a vitrine faz **uma captura de cena por quadro** (512², só o ator da lista) enquanto o menu está aberto | 🟡 | Medir no alvo fraco; se preciso, desligar por plataforma (`bVeuLigado`, `Runner.Veil 0`) e/ou reduzir a captura (LadoDaCaptura) ou capturar sob demanda |
| `PROC-004` | Alvo **multi-hardware** (PC → mobile → web) ainda não está registrado como restrição de arquitetura | 🔴 | `Docs/Target_Hardware.md` com a lista do que é proibido usar por causa disso |

---

## NÃO é issue — é DECISÃO pendente

> **Fonte única das decisões abertas:** `Docs/PloidrekRPG_GDD_v3.md` **§15**. A lista abaixo é histórica (decisões que
> existiram durante a consolidação); use a do v3 como referência vigente.

Estas travaram a escrita do **GDD v3** e precisam de escolha humana (não de execução):

| ID | Decisão pendente | Impacto |
|---|---|---|
| `DEC-01` | Nome do fenômeno (névoa) e dos corrompidos | Bloqueia lore, HUD, tags, nomes de asset |
| `DEC-03` | Fórmula de HP (dado de vida por classe + mod CON?) e escala por nível | Bloqueia TTK e todo o QC do bestiário |
| `DEC-05` | Lista de **proficiências** e o que cada uma destrava | É o eixo de "diferença por acesso" |
| `DEC-06` | Estrutura dos **nós de talento** (nº de nós, 3 árvores, o que muda) | Bloqueia progressão e UI |
| `DEC-10` | No fallback do auto-reparo, "cura = Éter atual" **gasta** todo o Éter restante (conversão 1:1) ou é gratuita? | Fecha a fórmula e evita cura grátis |
| `DEC-11` | Teto de **4 ataques** por Ação de Ataque, inclusive no épico | Protege orçamento de animação e de rede |
| `DEC-12` | ✅ **resolvido:** existe desvantagem além do alcance normal (5e); ignorá-la é vantagem de modelo (Overcore) | — |
| `DEC-13` | ✅ **encerrado:** "capacidade de carga" era fraca e foi substituída por **Peso Morto** (imune a empurrão/derrubada) | — |
| `DEC-15` | Vocabulário de tempo: **turno = 10 rodadas (60 s)**, descanso curto 10 min, longo 20 min | Define todas as durações |
| `DEC-14` | Lista de **testes** fora de combate (o trait social dá "+1d4 em testes") | Amarra com `DEC-05` (proficiências) |
| `DEC-08` | Morte/downtime/respawn | Explicitamente fora do MVP — registrar como adiado |
