# Inventário — VoltStriker (protótipo) e material com termo de IP

> **O que é o VoltStriker:** um **teste**. Ele não tem relação com o design atual do jogo — foi o resultado da tentativa de
> usar IA em todo o fluxo de geração de assets (ComfyUI → Blender → Unreal) na fase de reconhecimento das ferramentas
> MCP. A **malha 3D é descartável**. O que tem valor é parte do **código** que foi escrito para fazê-la funcionar.

---

## 1. Código C++ — `Source/AI_MEGA_MAN_TEST/` (11 classes)

| Classe | O que é | Veredito |
|---|---|---|
| `UVoltStrikerAttributeSet` | AttributeSet de GAS (Health, MaxHealth, Energy, AttackPower, ChargeLevel, MoveSpeed, Damage) | **reaproveitável** — é a base do recurso do jogo |
| `UVoltStrikerGameplayAbility` | classe-base de habilidade (custo, CD, tags) | **reaproveitável** |
| `UGA_BasicShot` · `UGA_ChargeShot` · `UGA_RapidShot` · `UGA_DashShot` · `UGA_JumpShot` | 5 habilidades implementadas | **reaproveitável como referência** — mapeiam direto no kit do **Blaster** (tiro básico, carregado, rápido, dash, aéreo) |
| `AVoltStrikerCharacter` | pawn com Enhanced Input + ASC | **reaproveitável como base**, precisa ser reescrito como `ARunnerCharacter` |
| `AVoltStrikerWeapon` | arma modular, sockets, `SetChargeVisual(0..1)` | reaproveitável |
| `AVoltStrikerProjectile` · `AVoltStrikerChargeProjectile` | projétil simples e projétil carregado (perfurante/AoE) | reaproveitável |
| `AVoltStrikerCameraManager` | câmera / pitch terceira pessoa | reaproveitável |
| `UVoltStrikerAnimInstance` | anim instance C++ | reaproveitável |
| `AVoltStrikerGameMode` | game mode do protótipo | descartável (o do jogo será outro) |
| `ICombatInterface` | interface de combate (dano/aplicar efeito) | **reaproveitável** |

**Conclusão:** existe um **scaffold de GAS funcionando** (ASC + AttributeSet + 5 habilidades + projéteis + interface).
É a parte mais valiosa do protótipo e **não depende do modelo 3D nem do nome VoltStriker**.

## 2. Conteúdo Unreal — `Content/VoltStriker/` (**67 arquivos**)

| Pasta | Qtd | Conteúdo | Veredito |
|---|---|---|---|
| `Blueprints/` | 7 | ABP_VoltStriker · BP_PlayerRobot · BP_Weapon · BP_Projectile · BP_ChargeProjectile · BP_CameraManager · BP_VoltStrikerGameMode | **descartável** (reescritos sobre o scaffold novo) |
| `Input/` | 9 | 8 × `IA_*` (Fire, Jump, Look, Move, MoveForward/Back/Left/Right) + `IMC_VoltStriker` | **reaproveitável** como ponto de partida dos Input Actions |
| `Mesh/` | 26 | `SKM_VoltStriker` + **7 variantes** (Src, Play, CM, V2–V6) e **5+ skeletons**, physics asset, 6 materiais `M_VS_*` | **descartável** — é exatamente o "versionamento fora de controle" (`ORG-003`) |
| `Rig/` | 1 | `CR_VoltStriker` | descartável |
| `Textures/` | 24 | 6 mapas × 2 resoluções (2K/4K) + PNGs dentro do Content | **descartável como textura de jogo** (é lixo o 4K dentro do Content — `ORG-004`) |

## 3. Arte-fonte — `Art/VoltStriker/` (16 arquivos)
`SKM_VoltStriker.fbx` · `VoltStriker.blend` · `VoltStriker.blend1` · 12 PNGs (BaseColor, Normal, Roughness, Metallic, AO,
Emission em 2K e 4K). **Veredito: descartável** — mas é o **único registro de que o pipeline IA→FBX→UE funcionou ponta a ponta**.
Sugestão: manter **só o `.blend` e uma textura 2K** como prova de fluxo, e o resto fora.

## 4. Documentação (6 docs)
`CharacterDesign.md` · `Abilities.md` · `Weapon.md` · `AnimationPipeline.md` · `ImportPipeline.md` · `Gameplay.md`.
**Veredito: híbrido.** `Abilities` e `Weapon` são **boas referências técnicas** de GAS/sockets. `ImportPipeline` e
`AnimationPipeline` são **procedimento real do pipeline** (continuam válidos). `CharacterDesign` e `Gameplay` descrevem
o **protótipo** e ficam como histórico.

## 5. Outros
- `Tools/Blender/build_voltstriker.py` — script que gerou o mesh. **histórico** (o pipeline real é `AI_Pipeline/`).
- `Captures/voltstriker_pie.png` — print do PIE. **histórico**.
- `Config/DefaultEngine.ini` → `GlobalDefaultGameMode=/Game/VoltStriker/Blueprints/BP_VoltStrikerGameMode`. ⚠️ **é a
  referência ativa**: mover ou renomear `Content/VoltStriker` **quebra o projeto** sem *Fix up Redirectors*.

---

## 6. Material com nome de IP (referência de estudo)

| Arquivo | Situação |
|---|---|
| `Art/Megaman.obj` (22 MB) · `Art/Megaman.stl` (69 MB) | modelo de estudo — **não pode entrar no build** |
| `Art/reference/Megaman.fbx` (7 MB) | idem |
| `Art/generated/world/reploid.png` · `maverick.png` | arte gerada com nome de termo de IP — **renomear** |
| `Art/Classes/sample_maverik.jpg` | sample antigo de classe — nome vem de roster descartado |

**Não foram movidos nem apagados** (podem estar em uso por workflows do ComfyUI). A pasta `Art/reference/_ip/` foi criada
para recebê-los quando você quiser organizar — é só mover e renomear. Regra: **referência fica em `Art/`, nunca em
`Content/`**, e passa pelo manifesto de proveniência (`IP-005`).

---

## 7. O que decidir

| # | Decisão | Impacto |
|---|---|---|
| 1 | **Reaproveitar o scaffold GAS** (AttributeSet + 5 habilidades + projéteis + interface) renomeando para o jogo, ou escrever do zero | Economia grande de tempo — o kit do Blaster já existe em código |
| 2 | **Arquivar `Content/VoltStriker/`** (com Fix up Redirectors) ou deixar como está até o scaffold novo existir | Limpeza do Content Browser vs. risco de quebrar referência |
| 3 | **Input Actions** (`IA_*`) entram como base do mapeamento novo? | Evita retrabalho de Enhanced Input |
| 4 | Guardar **1 prova de fluxo** do pipeline IA→UE (`.blend` + 1 textura) antes de descartar o resto | Registro de que o pipeline funcionou |
