# Plano de Renomeação — VoltStriker* → Runner*

> **Decisão (`ARQ-001`):** reaproveitar o scaffold GAS do protótipo, renomeando para a nomenclatura do jogo.
> Escopo: **nada de arte** — a malha, texturas e rig do VoltStriker são descartáveis. O que se aproveita é **código e
> configuração** (11 classes C++, Input Actions e o padrão de pastas).

---

## 1. Regra de ouro do prefixo

**`Runner*` só onde a coisa *é* de um Runner.** O resto usa o prefixo do jogo (**AF**), porque daqui a três meses um
`ARunnerGameMode` seria simplesmente errado — o game mode não é um Runner, ele governa o mundo.

## 2. C++ — `Source/AI_MEGA_MAN_TEST/`

| Atual | Novo | Porque |
|---|---|---|
| `AVoltStrikerCharacter` | **`ARunnerCharacter`** | é o pawn do Runner |
| `UVoltStrikerAttributeSet` | **`URunnerAttributeSet`** | atributos (HP, Éter) do Runner |
| `UVoltStrikerAnimInstance` | **`URunnerAnimInstance`** | animação do Runner |
| `AVoltStrikerCameraManager` | **`ARunnerCameraManager`** | câmera do jogador |
| `UVoltStrikerGameplayAbility` | **`UAFGameplayAbility`** | base do **jogo**, não de um Runner |
| `AVoltStrikerProjectile` | **`AProjectileBolt`** | projétil não é um Runner |
| `AVoltStrikerChargeProjectile` | **`AProjectileCharge`** | idem |
| `AVoltStrikerWeapon` | **`AWeaponBase`** | arma é item; tipos entram por BP / `DA_` |
| `AVoltStrikerGameMode` | **`AAFGameMode`** | governa o mundo |
| `ICombatInterface` | **manter** | já é neutro e genérico |
| `UGA_BasicShot` · `UGA_ChargeShot` · `UGA_RapidShot` · `UGA_DashShot` · `UGA_JumpShot` | **manter** | já seguem a convenção do playbook §8.2 (`GA_` para Gameplay Ability) |

## 3. Assets e pastas em `Content/`

| Atual | Novo |
|---|---|
| `Content/VoltStriker/` | **`Content/Runners/`** |
| `BP_PlayerRobot` | **`BP_Runner_<Chassi>`** (ex.: `BP_Runner_Vitaspark`) |
| `ABP_VoltStriker` | **`ABP_Runner`** |
| `IMC_VoltStriker` | **`IMC_Game_KBM`** (+ `IMC_Game_Gamepad`, `IMC_Game_Touch` — playbook §4.1) |
| `IA_*` (8) | **manter** (já no padrão do playbook) |
| `CR_VoltStriker` | **`CR_Runner`** (compartilhado; por chassi só se a proporção mudar) |
| `SKM_VoltStriker` + 7 variantes + 5 skeletons | **não renomear — arquivar** (`ARQ-002`) |
| `M_VS_*` (6 materiais) | **`M_Runner_*`** |
| `DT_PloidrekModels` / `DT_PloidrekClasses` | **`DT_Chassis`** / **`DT_Classes`** (`STR_Chassis`, `STR_Class`) |
| `BP_VoltStrikerGameMode` | **`BP_AFGameMode`** |

Árvore alvo do `Content/` (combinando com `OrganizationAnalysis` §3):

```
Content/
├── Runners/            Characters/Runners/<Chassi>/ · Input/ · Anim/ · Rig/
├── Abilities/          GA_*, GE_*, Cues
├── Weapons/            BP_Item_*, DA_Weapon_*
├── Enemies/            Apagados · Clyffen · fauna mecânica
├── Data/               DT_Chassis · DT_Classes · DT_Feats · DT_Abilities · DT_Enemies
├── Maps/               World/Nexus7 + setores
└── UI/                 CommonUI
```

## 4. Ordem de execução (importa)

1. **Primeiro o nome do módulo/projeto** — ver §5. É a renomeação mais barata **agora** e a mais cara depois.
2. **C++**: renomear classes e os arquivos de cabeçalho e implementação, mais o macro `AI_MEGA_MAN_TEST_API`. Compilar limpo.
3. **Reabrir o editor** e **reparentar** os Blueprints para as classes novas (`BP_CameraManager`, `BP_Weapon`,
   `BP_Projectile`, `BP_ChargeProjectile`, `BP_PlayerRobot`, `ABP_VoltStriker`) — **sem isso os BPs ficam órfãos**.
4. **Assets**: renomear **dentro do editor** (nunca pelo sistema de arquivos), com **Fix up Redirectors** ao final.
5. **`Config/DefaultEngine.ini`**: apontar `GlobalDefaultGameMode` para o game mode novo (`ARQ-005`).
6. **Arquivar** `Content/VoltStriker/Mesh/` e `Rig/` antigos (`ARQ-002`) e mover os **PNGs 2K/4K** que estão dentro de
   `Content/` para `Art/` (`ARQ-003`).
7. **Smoke test**: abrir mapa → spawnar → atirar → carregar tiro → aplicar dano.

> ⚠️ **Renomear `.uasset` pelo Finder quebra referências silenciosamente.** Sempre pelo Content Browser, com o editor
> aberto e *Fix up Redirectors* no fim.

## 5. Decisão pendente: nome do módulo e do projeto

Hoje o módulo é **`AI_MEGA_MAN_TEST`** — nome de **laboratório**, que aparece no `.uproject`, nos `Target.cs`, no macro
`AI_MEGA_MAN_TEST_API` de todas as 11 classes e no nome do binário. **Renomear depois de existir conteúdo é caro; agora é barato.**

### ✅ Decidido: o módulo/projeto se chama **`PloidrekRPG`**

Hierarquia de nomes (ver `Docs/PloidrekRPG_GDD_v3.md` §0.1):

| Nível | Nome |
|---|---|
| **Jogo** | **`PloidrekRPG`** ← nome do módulo, do `.uproject`, do binário e do launcher |
| **Universo** | **Aether Forge** (o universo do Éter) — **só na lore** |
| **Campanha inicial** | *Protocol Zero* (a confirmar) |
| **Mundo** | **Nexus-7** |

O macro C++ passa de `AI_MEGA_MAN_TEST_API` para **`PLOIDREKRPG_API`**, e o módulo de `AI_MEGA_MAN_TEST` para
**`PloidrekRPG`** nos `.uproject`, nos `Target.cs` e nos `Build.cs`.

---

## 6. Checklist

- [ ] Nome do módulo/projeto decidido (`PloidrekRPG` × `AetherForge`)
- [ ] C++ renomeado e compilando
- [ ] Blueprints reparentados
- [ ] Assets renomeados no editor + Fix up Redirectors
- [ ] `GlobalDefaultGameMode` retargetado
- [ ] Mesh/Rig legados arquivados; PNGs fora do `Content/`
- [ ] Smoke test (mapa → spawn → tiro → carga → dano)
- [ ] Documentação atualizada (`Abilities.md`, `Weapon.md`, `Gameplay.md` com os nomes novos)
