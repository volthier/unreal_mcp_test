> **Portado do canônico** (`/Users/Shared/ASHES/git/UNREAL/PloidrekRPG/Docs/ENGINEERING_PLAYBOOK_UNREAL.md`, somente leitura).
> Fecha o **MIG-003** do `Docs/BACKLOG_Tecnico.md`: a versão citada foi corrigida de **5.7 para 5.8**, que é a engine
> deste repositório.
>
> **Onde este documento manda no jogo:** a seção **4 (Input & UI cross-platform)** e a **5 (Combate LoL + leitura Albion
> + feel MegaMan — top-down 3D)** são a base do `Docs/Modelo_Interacao.md`, que detalha câmera, targeting,
> locomoção por clique e a convivência entre a janela de rodada de 6 s e o tempo real.

# Unreal Engine 5.8 — AI Dev Agent Guardrails (Boas Práticas Consolidadas)
## Open World Sandbox MMORPG (Albion-like) + Combate LoL (QWER) + “MegaMan 3D” Feel
### DDD pragmático + Clean Code + Clean Architecture + padrões Unreal

Este documento define **guardrails** para um **agente de desenvolvimento IA** contribuir com segurança em um projeto Unreal Engine, evitando o ciclo: *“funciona até quebrar tudo”*.

> Meta: **consistência + ownership + automação**.  
> Não é “DDD em tudo”. É reduzir entropia, aumentar previsibilidade e permitir escala.

---

## 0) Regras de Ouro (o agente deve obedecer)

### MUST
1. **Stability > Churn**  
   Se o projeto está instável, **pare features** e estabilize antes de refatorar grande.
2. **Mudanças pequenas e reversíveis**  
   PRs pequenos, foco único, fácil review e rollback.
3. **Boundaries primeiro**  
   Respeite limites de módulos/plugins. **Não misture UI com regra do jogo**.
4. **Automação é o guardrail real**  
   Sem CI/Automation, padrões viram opinião. **Fortaleça CI sempre**.
5. **Diagnóstico antes de reescrever**  
   Nunca “refaz do zero” sem evidência, reproduções e critérios de sucesso.

### SHOULD
- Preferir **design incremental**: refatorar por “estrangulamento” (strangler pattern), não por big-bang rewrite.
- Introduzir **métricas** (perf, rede, memória, build time) antes de otimizar.

### MUST NOT
- Criar “atalhos” que furam arquitetura (ex.: UI aplicando dano, Blueprint gigante, singletons globais soltos).
- Fazer PR “limpeza geral” junto com feature/bugfix (mistura aumenta risco).

---

## 1) Arquitetura-alvo (DDD + Clean Architecture pragmático no Unreal)

### 1.1 Camadas (conceitual)
**Domain (Core)**
- Regras de negócio, modelos, invariantes.
- Idealmente **sem depender de UMG/Actors/UWorld**.
- Usa Value Objects/Entities/Aggregates, Domain Services, Domain Events.

**Application (Use Cases)**
- Orquestra fluxos: “Iniciar combate”, “Aplicar drop”, “Finalizar craft”.
- Valida permissões, dispara efeitos, chama infraestrutura via interfaces.

**Infrastructure**
- Persistência, rede, integração com Engine, DB/HTTP, serialização.
- Implementa interfaces da Application/Domain.

**Presentation**
- UI/HUD/Widgets, input, feedback visual, estados de tela.

> Regra: **Dependências sempre apontam para dentro** (UI → App → Domain).  
> Engine/Unreal é, em geral, “Infra/Adapters”.

### 1.2 Bounded Contexts (DDD prático)
Defina limites claros (ownership por contexto):
- **Combat** (GAS, habilidades, status, dano)
- **World** (World Partition, streaming, spawn rules, POIs)
- **Items/Economy** (inventário, craft, loot, mercado)
- **Networking** (replicação, interesse, sessão, mensagens)
- **Persistence** (save, schema versioning, migração)
- **UI/UX** (HUD, menus, CommonUI)

> Regra: um contexto **não importa** diretamente classes internas do outro; use interfaces, eventos ou contratos.

---

## 2) Layout recomendado no Unreal (estrutura do projeto)

### 2.1 Source (C++)
- `Source/<GameName>/` → módulo principal (runtime)
- `Source/<GameName>Editor/` → **somente tools de editor**
- `Source/<GameName>Tests/` → automation/functional tests helpers

### 2.2 Plugins (boundaries claros)
Use Plugins quando um sistema é grande o suficiente para ter ownership.

Exemplo:
- `Plugins/GameCore/Source/GameCore/`  
  **Domain + Application contracts** (sem UI)
- `Plugins/GameWorld/Source/GameWorld/`  
  World Partition, streaming helpers, spawn orchestration
- `Plugins/GameCombat/Source/GameCombat/`  
  GAS wrapper, abilities, attributes, combat services
- `Plugins/GameNetworking/Source/GameNetworking/`  
  interesse/replicação, mensagens, métricas
- `Plugins/GameUI/Source/GameUI/`  
  CommonUI, HUD, menus, widgets
- `Plugins/GamePersistence/Source/GamePersistence/`  
  save, schema, migração, serialização

> Regra: **Editor module nunca** é dependência do runtime.

### 2.3 Content (assets)
Crie namespace único do projeto:
- `Content/_Project/`
  - `Input/`
  - `Config/`
  - `Blueprints/`
  - `UI/`
  - `Maps/`
  - `Materials/`
  - `FX/`
  - `Audio/`
  - `Data/` (DataAssets, DataTables)

**Regra:** assets de sistema não ficam espalhados em `Content/Random`.

---

## 3) Open World Seamless + MMO: guardrails técnicos essenciais

### 3.1 World Partition / Streaming (sem loading screen)
**MUST**
- Usar **Persistent Level + World Partition** com streaming runtime.
- Usar **HLOD** (obrigatório) para reduzir custo em longa distância.
- Organizar conteúdo com **Data Layers** (biomas, cidades, eventos, interiores).

**MUST NOT**
- Implementar “mundo infinito” manual ignorando WP, salvo casos específicos (ex.: instâncias/arenas/dungeons isoladas).

### 3.2 Dedicated Server + servidor não pode carregar “tudo”
**MUST**
- MMO-like exige **Dedicated Server**.
- Configurar/validar **server streaming** e consumo de memória/CPU com múltiplos players.

### 3.3 Networking: escala e previsibilidade
**MUST**
- Implementar **interest management**: o cliente só recebe o que está no raio/visão/necessidade.
- Separar mensagens/contratos e versionamento (principalmente para mobile).

**MUST NOT**
- Replicar “tudo para todos”.

---

## 4) Input & UI cross-platform (KBM + Controle + Touch) — UMA base de gameplay

### 4.1 Enhanced Input (camada única de ação)
**MUST**
- Definir **Input Actions** device-agnostic:
  - `IA_Move` (Vector2)
  - `IA_Aim` (Vector2)
  - `IA_PrimaryAttack` (Bool)
  - `IA_Ability_Q/W/E/R` (Bool)
  - `IA_Dash`, `IA_Jump`, `IA_Interact`, `IA_Cancel` (Bool)
  - `IA_Zoom` (Axis1D)
  - `IA_OpenMenu`, `IA_Inventory` (Bool)

**MUST**
- Criar **Input Mapping Contexts**:
  - `IMC_Game_KBM`
  - `IMC_Game_Gamepad`
  - `IMC_Game_Touch`

**MUST NOT**
- Hardcode de tecla dentro de habilidade/regra do jogo.

### 4.2 CommonUI (UI consistente e navegável)
**MUST**
- UI deve suportar navegação por controle e touch.
- UI **só lê estado** e **envia intenção** (commands). Não aplica resultados.

---

## 5) Combate LoL + leitura Albion + “feel” MegaMan (top-down 3D)

### 5.1 Targeting universal (funciona em todos inputs)
**MUST**
Todas habilidades com alvo seguem o mesmo fluxo:

1) **Press** habilidade → entrar em **Targeting Mode** + mostrar telegraph (cone/círculo/linha)  
2) Atualizar alvo:
   - KBM: cursor no mundo
   - Controle: aim-cursor com stick / direção+alcance
   - Touch: drag para direção/alcance
3) **Confirm/Release** → cast  
4) **Cancel** → abortar e retornar

**MUST NOT**
- Depender de mouse-only targeting.

### 5.2 Mobilidade “MegaMan 3D” sem destruir legibilidade
**MUST**
- Dash/jump com cooldown/custo e regras claras.
- Evitar verticalidade caótica que quebre leitura PvP top-down.

---

## 6) Habilidades e status: GAS como padrão

### 6.1 GAS obrigatório para QWER e status
**MUST**
- Usar GAS para: habilidades QWER, cooldown, custo, buffs/debuffs, stun/silence/slow/shield/DoT, stacks.
- Definir Attributes e Gameplay Effects com ownership claro.

### 6.2 Binding por Tags (Lyra-like)
**MUST**
- Tags: `Input.Ability.Q`, `.W`, `.E`, `.R`
- Enhanced Input dispara intenção → ASC resolve e ativa.

**MUST NOT**
- Duplicar “habilidade Q” por plataforma.

### 6.3 Clean boundaries no combate
- Ability executa lógica e dispara efeitos.
- Effects modificam atributos e estados.
- UI só exibe (cooldowns, recursos, estado).
- Networking valida e replica o mínimo necessário.

---

## 7) SOLID no Unreal (sem religião)

### SRP
- `Spawner` spawna; não decide score final nem mexe na UI.
- `UI Widget` apresenta estado; não decide dano, drop ou crafting.

### DIP
- Parâmetros em **DataAssets** (sem magic numbers).
- Injeção via Editor: `TSubclassOf<>`, propriedades editáveis.
- Subsystems/Services com interfaces (evitar singletons soltos).

### OCP
- Adicionar inimigo/arma/skill por config/classe nova, sem reescrever o core.

---

## 8) Standards (padrões obrigatórios)

### 8.1 C++ Style
- Padrão Epic/UE (prefixos `U/A/F/E`, `b` boolean).
- Forward declares em headers quando possível; includes mínimos.
- Logs com categorias por sistema; sem spam.

### 8.2 Blueprint Style
- Prefixos:
  - `BP_` Actor/Pawn
  - `WBP_` Widget
  - `DA_` DataAsset
  - `GA_` Gameplay Ability
  - `GE_` Gameplay Effect
  - `AS_` AttributeSet
- Blueprint é **composição/wiring**; lógica crítica fica em C++/Core quando necessário.

### 8.3 Naming e pastas
- Sem `final2`, `newnew`, `temp`.
- Asset não troca de pasta “porque sim”.

---

## 9) Configs e parâmetros: DataAssets como guardrail

**MUST**
- Externalizar tuning para DataAssets:
  - move speed, cooldowns, intervalos, limites
  - score/loot rules
  - referências de classes (`TSubclassOf`)
- Versionar schemas de dados quando evoluir (ex.: itens/economia).

---

## 10) Tests e Automation (Unreal)

### 10.1 Prioridade (ordem)
1. Networking
2. Persistence/Save
3. Core gameplay loop (spawn → ação → recompensa → game over)
4. Regressões de performance (spikes, leaks, GC)

### 10.2 Smoke Tests mínimos (obrigatório)
Automation/Functional tests devem garantir:
- Abre mapa `Main`
- Player spawna
- UI renderiza
- 1 inimigo spawna
- Ação básica funciona (attack/colisão)

### 10.3 Quando usar Functional Tests
- Para validar fluxo completo real (mundo, nível, atores, replicação).

---

## 11) CI / Build Pipeline (guardrail real)

### 11.1 Pipeline mínimo
- Build Editor + Game (plataformas-alvo)
- Rodar Smoke Tests (subset rápido)
- Lint/format quando aplicável
- Publicar artifacts (logs + report)

### 11.2 Gates (sem exceção)
- Sem merge se build falhar
- Sem merge se smoke falhar
- Sem merge se warnings críticos aumentarem

---

## 12) PR Checklist (o agente deve cumprir)

- [ ] Build local/CI passa
- [ ] Sem warnings novos relevantes
- [ ] Smoke test executado
- [ ] Mudança tem foco único (sem “cleanup geral” junto)
- [ ] Boundaries respeitados (Core/UI/Gameplay/Editor)
- [ ] Parâmetros em DataAssets/config (sem magic numbers novos)
- [ ] Naming/pastas padrão
- [ ] Documentação mínima atualizada se mudou fluxo
- [ ] Para features de combate: telegraph/targeting funciona em KBM + Gamepad + Touch

---

## 13) Templates por área (reduzir caos)

O agente deve criar e manter exemplos canônicos para:

### 13.1 Networking
- Mensagens/contratos + versionamento
- Logs e métricas
- Retry/backoff, reconexão

### 13.2 Persistence / Save
- Schema versioning + migração
- Integridade e fallback
- Testes automatizados

### 13.3 Gameplay Systems
- Registro de sistemas
- Eventos principais
- Onde guardar estado
- Parametrização via DataAssets

### 13.4 UI
- Binding padrão
- Updates via eventos (evitar polling eterno)
- Estados: loading/playing/gameover

### 13.5 Tools/Build
- Scripts repetíveis
- Setup de engine version
- CI docs

---

## 14) Anti-patterns (proibidos)

- Reescrever infraestrutura “do zero” sem diagnóstico
- Misturar UI com regra de gameplay
- Hardcode de parâmetros em C++/BP (sem justificativa)
- Refactor gigante sem testes
- PRs enormes e irrevisáveis
- Dependências circulares entre módulos/plugins
- Ignorar warnings e regressões
- Duplicar lógica por plataforma

---

## 15) Definition of Done (DoD)

Uma tarefa está pronta quando:
- Compila e roda
- Não quebra smoke tests
- Boundaries/padrões respeitados
- Mudança é pequena e rastreável
- Configs/valores externalizados quando aplicável
- Risco do sistema diminuiu (não aumentou)
- (Cross-platform) funciona em KBM + Gamepad + Touch quando relevante

---

## 16) Nota sobre DDD em Unreal (uso consciente)

DDD ajuda quando:
- Regras são complexas e mudam muito (economia, inventário, progressão, craft, marketplace).
Não force DDD em:
- UI
- Actors simples e efêmeros
- wiring de cena

> Foco: **consistência + ownership + automação**. DDD é ferramenta, não religião.

---

## 17) “Agent Operating Procedure” (como agir antes de codar)

**MUST**
1. Reproduzir o problema (passos + logs + cenário mínimo)
2. Formular hipótese + risco
3. Implementar correção mínima e reversível
4. Adicionar/ajustar teste (smoke/functional) quando possível
5. Validar em CI e anexar evidências (log, screenshot, perf)

**MUST NOT**
- “Refazer tudo” para “ficar bonito” sem prova de necessidade.
