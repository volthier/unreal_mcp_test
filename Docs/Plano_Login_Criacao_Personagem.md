# Plano — Login → Chassi → Classe → Corpo (manequim da engine)

> **Objetivo:** trazer o **login** que já existe no canônico (PloidrekRPG), **ligar** isso à **seleção de chassi** e depois à
> **seleção de classe**, e fazer o personagem nascer já com um **corpo utilizável** — usando o **manequim padrão da engine**
> como placeholder até existir arte própria.
> **Projeto:** este repositório (UE **5.8**). **Canônico:** `/Users/Shared/ASHES/git/UNREAL/PloidrekRPG` (UE **5.5**, 100% Blueprint) — fonte de **somente leitura**.

---

## 1. O fluxo alvo

```
ABRIR JOGO
   └── TELA DE LOGIN            (usuário / senha)            [do canônico]
        ├── criar conta        (WBP_CreateAccount)          [do canônico]
        └── entrar
             └── SELEÇÃO DE CHASSI   (10 opções + slot EXTINTO)   [novo — DT_Chassis]
                  └── SELEÇÃO DE CLASSE (6 opções)               [novo — DT_Classes]
                       └── CRIA O PERSONAGEM
                            └── spawna o corpo (MANEQUIM da engine) + dados do chassi e da classe aplicados
```

**Regra de arquitetura (playbook §4.2 e §6):** a **UI só lê estado e envia intenção**. Toda a lógica — sessão, validação,
seleção pendente, criação — vive em **C++** (subsystem). Os widgets são finos.

## 2. O que o canônico já tem (levantado)

| Ativo | Onde | Uso |
|---|---|---|
| `WBP_Login` · `WBP_CreateAccount` · `WBP_CreateCharacter` | `Content/Blueprints/UI/` | o fluxo de entrada e criação |
| `BP_GameInstance` | `Content/Blueprints/Systems/` | estado global entre mapas (sessão do jogador) |
| `MainMenuMap` · `LogedInSelectionMap` | `Content/Maps/` | menu → seleção |
| `DT_PloidrekClasses` · `DT_PloidrekModels` · `STR_*` | `Content/Data/` | as 6 classes e os 10 modelos (a fonte dos nossos `DT_Classes`/`DT_Chassis`) |
| **Manequim completo** (meshes, rigs, animações, texturas) | `Content/Characters/Mannequins/` | ⚠️ está em **5.5** — ver §4 |

## 3. O que decidimos fazer aqui

| # | Passo | Como | Estado |
|---|---|---|---|
| 1 | **Corpo padrão** | copiar o manequim do **template da engine 5.8** (`Templates/TemplateResources/High/Characters`) para `Content/Characters/Mannequins/` | ✅ **feito** (128 assets, 126 MB, package path conferido) |
| 2 | **Dados canônicos** | `DT_Chassis` e `DT_Classes` com as estruturas em **C++** (`FRunnerChassisData`, `FRunnerClassData`), importadas de **CSV versionado** | ✅ **feito** — 11 chassis (10 + Clyffen extintos) e 6 classes, importados por script headless |
| 3 | **Subsystem do fluxo** | `URunnerSessionSubsystem`: conta (persistida em `Saved/RunnerAccounts.tsv`), seleção de chassi/classe com validação, `CreateCharacterProfile()` | ✅ **feito** |
| 4 | **Corpo aplicado** | `ARunnerCharacter::ApplyProfile()` aplica a **malha do chassi** (manequim da engine) no pawn | ✅ **feito** — falta o material de overlay para a tinta do chassi |
| 5 | **UI** | `URunnerMenuWidget` em **C++/UMG**: entrar → criar conta → escolher chassi → escolher classe → criar personagem. A lista de opções vem do **DataTable**, não de código | ✅ **feito** (sem estilo — o visual vem depois) |
| 5b | **Entrada no jogo** | `AMenuGameMode` abre o menu; ao criar, `OpenLevel` com `?game=/Script/PloidrekRPG.AFGameMode`; o pawn nasce e recebe o corpo | ✅ **feito** |
| 5c | **Persistência** | conta em `Saved/RunnerAccounts.tsv` e personagem em `Saved/RunnerCharacters.tsv` — entrar de novo restaura chassi e classe | ✅ **feito** |
| 6 | **Renomeação do projeto** | módulo `AI_MEGA_MAN_TEST` → `PloidrekRPG`; classes `VoltStriker*` → `Runner*` (ver `Plano_Renomeacao_Runner.md`) | ⏳ |

## 4. O problema de versão (5.5 → 5.8) e as duas saídas

`.uasset` **não** atravessa versão de engine por cópia de arquivo com segurança — especialmente **Blueprint e Widget**
(bytecode versionado). O manequim escapou disso porque veio **nativo 5.8** do template.

| Saída | Para widgets/BP do canônico | Recomendação |
|---|---|---|
| **A. Migração pelo editor** | abrir o Ploidrek no **5.8**, **Migrate** os assets para este projeto — o editor reescreve no formato novo | ✅ **caminho seguro** (exige o editor aberto, 5 minutos de trabalho manual) |
| **B. Cópia bruta + resave por commandlet** | copiar os `.uasset` e rodar o editor em `-run=pythonscript` para carregar e re-salvar | 🟡 automatizável, mas **pode falhar** em widget complexo |

**Regra:** tudo que é **dado** (DataTable, CSV, struct em C++) eu recrio aqui — não depende de migração. Tudo que é
**Blueprint/Widget** vem pela **saída A**, porque reescrever no 5.8 é mais barato e mais seguro do que consertar bytecode.

## 5. Decisão pendente do autor: login EOS ou local?

O canônico usa **EOS** (`OnlineSubsystemEOS` + `OnlineServicesOSSAdapter`). No 5.8 isso exige configuração de projeto na
Epic Games e uma conta de desenvolvedor.

| Opção | Prós | Contras |
|---|---|---|
| **Login local (dev) agora** | zero configuração; valida o fluxo inteiro já; serve o slice | não é login real de produção |
| **EOS agora** | já nasce "de verdade"; conta, sessão e amigos | configuração, dependência de serviço, e atrasa o slice |

**Recomendação:** **local agora, EOS depois** — o subsystem nasce com a interface de sessão separada, então trocar para EOS
depois é trocar a implementação, não reescrever o fluxo.

## 6. Roadmap por rodadas

| Rodada | Entrega | Verificação |
|---|---|---|
| **1** ✅ | **feito:** renomeação do projeto (módulo `PloidrekRPG`, classes `Runner*`) · build headless validado · manequim no projeto · structs de dado · CSVs · **DataTables importados** · este plano | build `Succeeded`; 11 chassis e 6 classes dentro dos `.uasset`; package paths conferidos |
| **2** ✅ | **feito:** BPs reparentados · `URunnerRules` (matemática) · `URunnerCharacterFactory` · `URunnerSessionSubsystem` · `ApplyProfile` · **suíte de automação** | `Runner.Regras.Matematica` e `Runner.Fluxo.ChassiEClasse` **Success** — 60 combinações validadas |
| 3 | subsystem do fluxo + `ARunnerCharacter` com manequim | personagem nasce com o corpo escolhido |
| **4 (atual)** | **acabamento**: estilo visual do menu (WBP por cima do widget C++), material de overlay para a cor do chassi, e o teste funcional no mapa | play no editor mostra o menu e entra no jogo com o corpo escolhido |
| **3** ✅ | **feito:** UI em C++/UMG, entrada no jogo, persistência da conta e do personagem, GameMode de menu | `Runner.Regras.Matematica`, `Runner.Fluxo.ChassiEClasse` e `Runner.Sessao.ContaECriacao` — **todos Success** |
| 6 | migração dos widgets do canônico (saída A) | login do canônico rodando no 5.8 |

## 6b. Como o fluxo roda hoje (passo a passo)

```
1. Abrir o projeto e dar Play em NewMap
   └ o WorldSettings do mapa aponta para BP_MenuGameMode (pai: AMenuGameMode)
     └ abre URUNNERMENUWIDGET: tela de login
2. Entrar (ou criar conta)                     [URunnerSession::Login / CreateAccount]
3. Escolher o chassi  (10 opções + os Clyffen aparecem como extintos e são recusados)
4. Escolher a classe  (6 opções)
5. "Criar personagem"                          [URunnerSession::CreateCharacterProfile]
   └ grava a ficha na conta e troca de GameMode:
     OpenLevel(NewMap, "?game=/Script/PloidrekRPG.AFGameMode")
6. AFGameMode::HandleStartingNewPlayer
   └ lê a ficha da sessão e chama ARunnerCharacter::ApplyProfile
     └ o pawn recebe a MALHA DO CHASSI (manequim da engine) + cor de destaque
```

**O que falta para ficar redondo:** o widget C++ não tem estilo (usa o visual padrão do Slate) — o
visual próprio entra depois, por WBP por cima ou por Style Set. E a cor de destaque do chassi só
aparece quando existir um **material de overlay** (os materiais do manequim não expõem parâmetro de cor).

## 7. Pendências que podem travar

| # | Item | Dono |
|---|---|---|
| 1 | **Permissão de escrita fora do workspace** para o UnrealBuildTool (`~/Library/Application Support/Epic/`) | autor — sem isso não há build headless |
| 2 | Abertura do **editor** para migrar os widgets do canônico (saída A) | autor |
| 3 | Decisão **login local × EOS** (§5) | autor |
| 4 | Nome das telas: manter `WBP_Login` ou renomear para o padrão do playbook (`WBP_Auth_Login`) | autor |
