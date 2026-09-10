# Análise de Organização — AI_MEGA_MAN_TEST

> Objetivo: analisar a organização do projeto e dos assets e identificar os ajustes necessários para um **MMORPG de mundo aberto**.
> **Fonte de design vigente:** `Docs/PloidrekRPG_GDD_v3.md` (universo **Aether Forge**, mundo **Nexus-7**).
> **Status:** análise histórica — parte das ações já foi executada (arquivo morto, renomeação planejada, consolidação do GDD).

## 1. Contexto do projeto

| Item | Valor |
|------|-------|
| Engine | Unreal Engine **5.8** |
| Módulo C++ | `AI_MEGA_MAN_TEST` (Runtime) |
| Plataforma alvo | Mac (também Win64/Linux nos plugins da engine) |
| Git | `github.com/volthier/unreal_mcp_test` (3 commits; foco no protótipo do personagem) |
| GDD | `Art/Aether_Forge_Protocol_Zero_GDD_v2.md` — MMORPG open world contínuo |
| Personagem | **VoltStriker** (robô de combate elétrico, GAS) |
| Mapa | `Content/NewMap.umap` (13 KB) — já usa **World Partition** (external actors + HLOD0) |

**Veredito geral:** o projeto é um protótipo de personagem + combate funcional (GAS, Enhanced Input, Blender→UE pipeline) sobre uma base **tecnicamente sólida** para um open world (World Partition, Lumen/RT, PCG, Substrate). A organização de `Content/` e `Art/`, porém, foi construída "por IP" e **não escala** para um MMORPG com dezenas de personagens, inimigos, setores e milhares de assets gerados por IA.

---

## 2. Achados — `Content/`

### 2.1 Estrutura atual

```
Content/
├── NewMap.umap, NewMap_HLOD0_Instancing.uasset
├── __ExternalActors__/NewMap/...        (World Partition — ok)
├── Characters/BP_MegaManX.uasset         ← stray/duplicado
├── Collections/, Developers/volthier/    (vazios/uso pessoal)
├── MegamanX/
│   ├── Blueprints/ (BP_MegamanX, BP_MegamanXGameMode)
│   ├── Materials/  (10 MI_*)
│   └── Mesh/  (8 M_*, skeleton, physics, SKM_MegamanX + .fbx)
└── VoltStriker/
    ├── Blueprints/ (ABP, BP_PlayerRobot, BP_Weapon, BP_Projectile, BP_ChargeProjectile, BP_CameraManager, BP_VoltStrikerGameMode)
    ├── Input/      (8 IA_* + IMC)
    ├── Mesh/       (6 M_*, 8 SKM_* variantes, 5 skeletons, physics asset, *.fbx fonte)
    ├── Rig/        (CR_VoltStriker)
    └── Textures/   (12 pares 2K/4K: BaseColor, Normal, Roughness, Metallic, AO, Emission)
```

### 2.2 Problemas identificados

| # | Problema | Impacto para MMORPG | Ação |
|---|----------|---------------------|------|
| 1 | **Organizado por IP** (`MegamanX` ≈ `VoltStriker`) em vez de por *tipo* | Não permite escalar nomeação/descoberta; conflito de nomes | Migrar para árvore por tipo (ver §3) |
| 2 | **BP duplicado** `BP_MegaManX` em `Characters/` e `MegamanX/Blueprints/` | Dois "donos", risco de divergência | Manter canônico em `Characters/MegamanX/`, remover o stray |
| 3 | **8 variantes SKM** (base, Src, Play, V2–V6, CM) + **5 skeletons** | Versioning fora de controle; skeleton errado muda referências | 1 skeleton canônico + 1 mesh final + LODs; arquivar o resto |
| 4 | **Fontes `.fbx` dentro de `Content/`** (são arquivos DCC) | Suja o Content Browser; dupla fonte de verdade | Mover para `Art/<nome>/` (fora de `Content`) |
| 5 | **Texturas 4K importadas** (personagem instanciado muitas vezes) | MMORPG: RAM/VRAM; 4K sem ganho em personagem | Usar 2K no jogo; manter 4K como fonte em `Art/` |
| 6 | Mapa único `NewMap` sem hierarquia | Sem estrutura de mundos/setores | Renomear para `Maps/World/Nexus7` + submaps por setor |
| 7 | **Sem pastas** de Environment / Items / FX / UI / Data; GameplayTags só de combate | MMORPG não tem onde guardar conteúdo do mundo | Criar árvore mínima + DataTables + tags |
| 8 | Git rastreia binários (blend, .blend1, fbx, PNG 4K); sem git-lfs | Repo cresce; `Art/` parcialmente untracked | git-lfs ou Plastic; `.gitignore` p/ fontes DCC |

### 2.3 Pontos positivos (manter)
- **World Partition já ativo** (external actors + HLOD) — base do mundo contínuo.
- **`Docs/` reais**: `ImportPipeline.md` (sockets/escala), `CharacterDesign.md` (LOD budgets), `Abilities.md`, `Weapon.md`, `Gameplay.md`, `AnimationPipeline.md`.
- **GAS** (asset attributes + tags), **Enhanced Input**, **Lumen/RT/Substrate**, **PCG** e plugins de **MetaHuman/Avalanche** presentes.
- **MCP já ligado**: engine MCP (UE 5.8) + `blender-mcp` — pipeline de import automatizável.

---

## 3. Ajustes necessários — MMORPG de mundo aberto

### Fase 1 — Estrutura de Content (fundação)

**Árvore alvo** (`/Game`):
```
/Game
├── Maps/                 World/Nexus7 (persistente) + setores
├── World/                Sectors/<setor>/, building props, PCG graphs
│   └── Sectors/
├── Characters/
│   ├── Heroes/VoltStriker/  Mesh, Textures, Materials, Animations, Audio
│   └── NPCs/                (inimigos, NPCs) — 1 skeleton por categoria
├── Enemies/              (Lordes de Setor, adds)
├── Items/                (armas, armaduras, materiais — + DataTables)
├── Weapons/
├── FX/                   (Niagara, Gameplay Cues)
├── UI/                   (CommonUI / UMG)
├── Data/                 (DT_*, CSV: itens, recipes, quests, factions)
├── Gameplay/             (GameModes, AI, quests, crafting, economia)
└── Dev/                  (só desenvolvimento; marcar p/ limpeza)
```

**Regras de naming/LOD/texturas** (convenções):
- Prefixos: `SM_` (static), `SKM_` (skeletal), `M_` (material), `MI_` (instance), `T_` (texture), `ABP_` (anim BP), `BP_` (blueprint), `IA_`/`IMC_` (input).
- **1 skeleton canônico** por categoria (heroes / NPCs); meshes compartilham skeleton.
- **Sem sources DCC em `Content/`** → tudo em `Art/`.
- LOD per `CharacterDesign.md`: LOD0 35k, LOD1 18k, LOD2 9k, LOD3 3k (personagem).
- Textura: personagem **2K** no jogo (4K como fonte), props 1–2K, terreno/mundo 2K+.
- Sockets padrão: `weapon_r`, `MuzzleFlash`, `ProjectileSpawn`, `FX` (per `Docs/ImportPipeline.md`).

> ⚠️ Migrar `Content/` exige **editor aberto** (quebra referências). Recomenda-se rodar com um feature-branch, mover com *Fix up Redirectors* e validar em PIE. Fora do escopo desta entrega.

### Fase 2 — Mundo / streaming
- Mapa canônico `Maps/World/Nexus7` (renomear `NewMap`).
- World Partition: cell size (grid) adequado à cidade (~10–25 m), HLOD por setor, **Data Layers** por setor/facção, **fog volumes** do Véu (sem submaps — transição sem costura).
- **PCG** para densidade da megacidade (já habilitado no `.uproject`).
- Orçamento: Lumen/RT ok para dev; definir hardware-alvo e preset de performance para cliente MMORPG.

### Fase 3 — Rede (maior gap)
- Hoje é **listen-server** (single-player-ish). MMORPG exige: **dedicated server**, save system, sessão/login, character select e persistência de estado do mundo.
- Item de projeto separado (arquitetura `GameServer`/`GameClient`, replicação por setor, Level Streaming, `OnlineSubsystem`).

### Fase 4 — Data-driven gameplay
- **DataTables**: itens, receitas de crafting, abilities, facções, quests, níveis de Véu.
- **GameplayTags** hierárquicos: expandir além de `Ability.*` → `Item.*`, `Craft.*`, `Faction.*`, `Quest.*`, `Sector.*`.

---

## 4. Resumo executivo

| Tema | Estado | Próximo passo |
|------|--------|---------------|
| Organização de `Content/` | ⚠️ por IP, com duplicação | Migrar p/ árvore por tipo (Fase 1) |
| Assets 3D do personagem | ✅ funciona, mas com versioning | Consolidar skel/mesh + LODs + 2K |
| World Partition | ✅ já ativo | Configurar setores/Data Layers/PCG |
| Rede | ❌ listen-server | Projeto dedicated server (Fase 3) |
| Gameplay data-driven | ⚠️ mínimo | DataTables + tags (Fase 4) |
| Pipeline IA→UE | ✅ MCP UE/Blender/Comfy ligados | Ver `Docs/AssetPipeline3D.md` |
