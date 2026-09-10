# Plano — Login (EOS) → Personagem → Chassi → Classe → Corpo (manequim da engine)

> **Objetivo:** trazer o **login** que já existe no canônico (PloidrekRPG), **ligar** isso à **seleção de chassi** e depois à
> **seleção de classe**, e fazer o personagem nascer já com um **corpo utilizável** — usando o **manequim padrão da engine**
> como placeholder até existir arte própria.
> **Projeto:** este repositório (UE **5.8**). **Canônico:** `/Users/Shared/ASHES/git/UNREAL/PloidrekRPG` (UE **5.5**, 100% Blueprint) — fonte de **somente leitura**.

---

## 1. O fluxo alvo

```
ABRIR JOGO
   └── TELA DE LOGIN            (conta / credencial)          [do canônico]
        ├── "Entrar (EOS — Dev Auth)"       → EOS Auth Interface (developer)
        ├── "Entrar com a conta Epic (EOS)" → EOS Auth Interface (accountportal)
        ├── "Criar conta nova (local)"      → reserva, quando não há EOS
        └── entrar
             └── JANELA PÓS-LOGIN: "escolha seu Runner"
                  ├── a conta TEM personagens → LISTA (selecionar um existente)
                  └── a conta NÃO tem       → segue para a criação
                       └── SELEÇÃO DE CHASSI   (10 opções + slot EXTINTO)   [DT_Chassis]
                            └── SELEÇÃO DE CLASSE (6 opções)               [DT_Classes]
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
| 5c | **Persistência** | conta em `Saved/RunnerAccounts.tsv`; **um personagem por linha** em `Saved/RunnerCharacters.tsv` (`conta\tid\tchassi\tclasse`) — a conta guarda **até 8 Runners** | ✅ **feito** |
| 5f | **Login EOS** | `URunnerSession::LoginWithEOS` chama a **Auth Interface** do `OnlineSubsystemEOS` (`IOnlineIdentity::Login`), com `AuthType` **`developer`** (Id/Token do Epic Dev Auth Tool) ou **`accountportal`** (conta Epic). Plugins `OnlineSubsystemEOS`/`OnlineSubsystemUtils`/`OnlineServicesOSSAdapter` + bloco `[OnlineSubsystemEOS.EOSSettings]` no `DefaultEngine.ini` com o **ambiente DEV** já registrado | ✅ **feito** |
| 5g | **Janela pós-login (novo ou existente)** | passo `CharacterSelect` do widget: a conta com personagens mostra a **lista** (selecionar um existente), a conta nova cai direto na criação (criar um novo); `SelectSavedCharacter` carrega a ficha escolhida | ✅ **feito** |
| 5d | **Cor do chassi** | `M_RunnerAccent` (overlay unlit/translúcido com o parâmetro `AccentColor`) aplicado por `ApplyProfile` via `SetOverlayMaterial` | ✅ **feito** — o manequim fica tingido com a cor do chassi |
| 5e | **Articulações (animação)** | `UpdateLocomotionAnimation`: idle / andar / correr / no ar, usando as animações **do próprio pacote do manequim** (`MM_Idle`, `MF_Unarmed_Walk_Fwd`, `MF_Unarmed_Jog_Fwd`, `MM_Jump`), configuráveis em *Project Settings*; a decisão é a função pura `URunnerRules::GetLocomotionState` | ✅ **feito** — até existir AnimBP próprio; o `bUseSingleNodeLocomotion` desliga isso quando o ABP existir |
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

## 5. Login: **EOS migrado do canônico**, local como reserva

**Decidido e feito:** o login do canônico foi migrado. A sessão tem as três portas e escolhe em execução.

| Porta | Como funciona | Quando usar |
|---|---|---|
| **EOS — Dev Auth** | `AuthType="developer"`, Id = nome da conta, Token = credencial gerada no **Epic Dev Auth Tool** | testar o login real sem abrir o navegador (o caminho de DEV do autor) |
| **EOS — Conta Epic** | `AuthType="accountportal"`, sem Id/Token — o engine abre o portal da Epic | validar o caminho de produção |
| **Local (reserva)** | `Saved/RunnerAccounts.tsv`, hash MD5 (grau de desenvolvimento) | quando o EOS não está disponível; mantém o slice jogável offline |

O botão de EOS só aparece quando `IsEOSAvailable()` é verdadeiro (plugin carregado **e** interface de identidade obtida);
sem EOS, o widget mantém a porta local visível.

**Configuração migrada** (`Config/DefaultEngine.ini`, ambiente **DEV** já registrado pelo autor):

```ini
[/Script/OnlineSubsystemEOS.EOSSettings]
DefaultArtifactName=Ploidrek
+Artifacts=(ArtifactName="Ploidrek", ClientId="...", ProductId="...", SandboxId="...",
            DeploymentId="...", ClientEncryptionKey="...")
```
`[OnlineSubsystem] DefaultPlatformService=EOS` · `[OnlineSubsystemEOS] bEnabled=true` · escopos BasicProfile, FriendsList, Presence.

> ⚠️ **Segredo não versionado.** O canônico traz o **`ClientSecret`** junto com os identificadores. Ele ficou
> **deliberadamente de fora** deste arquivo: é credencial de servidor, não pode ir para o cliente nem para o Git.
> Os **identificadores** (ClientId, ProductId, SandboxId, DeploymentId, EncryptionKey) são públicos por natureza e vieram.
> O Dev Auth **não precisa** do secret: o tool autentica com a **sua conta Epic**, não com credencial de cliente.
> Se algum dia o fluxo exigir o secret (troca de código / EOS Connect), ele entra em `Config/UserEngine.ini`
> (fora do versionamento).

## 5b. Dev Auth Tool no **macOS** (o login de desenvolvedor)

O tool **vem dentro do pacote do SDK**, na pasta `Tools/` — e o pacote traz o build de **macOS** junto com o de Windows:

```
SDK/Tools/EOS_DevAuthTool.app                     <- macOS (x86_64; roda sob Rosetta 2)
SDK/Tools/EOS_DevAuthTool-darwin-x64-1.2.1.zip    <- o mesmo app, empacotado
SDK/Tools/EOS_DevAuthTool-win32-x64-1.2.1.zip     <- Windows
```

> **O SDK não precisa ser instalado à parte.** O engine **5.8 já embarca o SDK 1.19.1.2** — exatamente a versão do
> pacote baixado — em `Engine/Binaries/ThirdParty/EOSSDK/Mac/libEOSSDK-Mac-Shipping.dylib` (conferido no
> `eos_version.h` do engine: 1.19.1.2). Do pacote baixado, o que se usa é **só o Dev Auth Tool**.

**Passo a passo**

1. Abrir o tool (Finder, ou pelo terminal):
   ```sh
   open "/Users/volthier/Documents/Freevoltz/EOS-SDK-IOS-53289219-Release-v1.19.1.2/SDK/Tools/EOS_DevAuthTool.app"
   ```
2. Escolher uma **porta TCP** para o tool ouvir os pedidos de login (ex.: `6547`).
3. Entrar com a **conta Epic Games de desenvolvedor** (e-mail, senha e MFA) — dentro do tool.
4. Dar um **nome** à credencial (ex.: `dev-volt`). **Uma credencial por conta Epic**: para testar
   multiplayer local são necessárias duas contas Epic.
5. No menu do jogo, no passo de entrada: **campo 1** = `localhost:6547` · **campo 2** = `dev-volt`.

**Por que esses dois campos?** É o que a doc oficial define para o tipo `EOS_LCT_Developer` (e é o que o
menu agora mostra como rótulo, em vez de "conta/senha"):

| Campo do menu | `EOS_Auth_Credentials` | Valor |
|---|---|---|
| campo 1 | `Id` | `localhost:<porta>` — host e porta onde o tool está ouvindo |
| campo 2 | `Token` | o **nome** dado à credencial no tool |
| (botão) | `Type` | `developer` |

> O tool **precisa ficar rodando** enquanto o jogo faz o login. Se ele estiver fechado, o `EOS_Auth_Login`
> falha e o motivo aparece no próprio status do menu e no `LogOnline`.

## 6. Roadmap por rodadas

| Rodada | Entrega | Verificação |
|---|---|---|
| **1** ✅ | **feito:** renomeação do projeto (módulo `PloidrekRPG`, classes `Runner*`) · build headless validado · manequim no projeto · structs de dado · CSVs · **DataTables importados** · este plano | build `Succeeded`; 11 chassis e 6 classes dentro dos `.uasset`; package paths conferidos |
| **2** ✅ | **feito:** BPs reparentados · `URunnerRules` (matemática) · `URunnerCharacterFactory` · `URunnerSessionSubsystem` · `ApplyProfile` · **suíte de automação** | `Runner.Regras.Matematica` e `Runner.Fluxo.ChassiEClasse` **Success** — 60 combinações validadas |
| **3** ✅ | **feito:** UI em C++/UMG, entrada no jogo, persistência da conta e do personagem, GameMode de menu | `Runner.Regras.Matematica`, `Runner.Fluxo.ChassiEClasse` e `Runner.Sessao.ContaECriacao` — **todos Success** |
| **4** ✅ | **feito:** material de overlay da cor do chassi + diagnóstico honesto do limite de verificação em execução | build e 3 suítes verdes; execução headless não inicia o jogo |
| **5** ✅ | **feito:** locomoção do corpo (idle/andar/correr/ar) + teste que spawna o personagem num mundo real | **5 suítes verdes**, incluindo `Runner.Corpo.MalhaEAnimacao` |
| **7** ✅ | **feito:** **login EOS migrado do canônico** (Auth Interface, Dev Auth + conta Epic, reserva local) e **janela pós-login** novo/existente com até 8 Runners por conta | **7 suítes verdes** — a nova `Runner.Sessao.VariosPersonagens` cria 2 Runners na mesma conta, lista, seleciona cada um e confere o isolamento entre contas |
| **8 (atual)** | **teste de Play do autor** com o EOS de DEV — a única coisa que a automação não alcança (§6c) | play mostra o menu, o login EOS responde, a janela lista os Runners e o corpo anda/corre/pula |
| **9** | **acabamento visual** do menu (WBP/Style Set) e migração dos widgets do canônico (saída A) | menu com a cara do jogo |

## 6b. Como o fluxo roda hoje (passo a passo)

```
1. Abrir o projeto e dar Play em NewMap
   └ o WorldSettings do mapa aponta para BP_MenuGameMode (pai: AMenuGameMode)
     └ abre URUNNERMENUWIDGET: tela de login
2. Entrar: "Entrar (EOS — Dev Auth)"           [URunnerSession::LoginWithEOS("developer", conta, credencial)]
   └ a Auth Interface do EOS responde → OnLoginComplete → a sessão toma o nome da conta EOS
   └ sem EOS: "Criar conta nova (local)" / entrar local   [CreateAccount / Login]
3. JANELA PÓS-LOGIN: "escolha seu Runner"
   ├── conta COM personagens → lista (R-01, R-02, ...) → jogar com o escolhido  [SelectSavedCharacter]
   │     └ pula direto para o passo 6 (chassi e classe vêm da ficha salva)
   └── conta SEM personagens → "Criar novo Runner" → segue para o passo 4
3b. Escolher o chassi  (10 opções + os Clyffen aparecem como extintos e são recusados)
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

## 6c. Limite de verificação (honesto)

Tentei **três vezes** rodar o jogo de forma headless para provar a corrente em execução (`-game -nullrhi`, com 45 s, 150 s e com `-log`). Nas três, o engine **inicia** ("Game Engine Initialized") mas
**não chega a iniciar o jogo** e não grava log capturável — inclusive matar o processo com SIGTERM perde o
stdout em buffer. Conclusão: **a execução de ponta a ponta só o autor pode confirmar**, com Play no editor.

O que **está** provado por automação: compilação limpa · **7 suítes de teste** (incluindo uma que **spawna o
personagem num mundo real** e confere malha, esqueleto e animação, e outra que cria **vários Runners na mesma conta**,
lista e seleciona cada um) · vínculos entre assets
(mapa → redirector → BP_MenuGameMode → `AMenuGameMode`, DataTables com as linhas certas, manequim com
package path correto).

**O que deve aparecer ao dar Play em `NewMap`:** o menu com o título *"PLOIDREKRPG — entrar"*, campo de conta,
campo de credencial e os botões **Entrar com o EOS (Dev Auth Tool)** / **Entrar com a conta Epic (EOS)** / **Criar conta nova (local)**.
Depois do login, a janela **"escolha seu Runner"** — a lista dos personagens da conta, ou o convite a criar o primeiro. No log:

```
LogTemp: AMenuGameMode: menu de entrada aberto (BP_MenuGameMode_C)
LogTemp: AFGameMode: ficha aplicada no pawn (chassi Vitaspark / classe Blaster)
LogTemp: ApplyProfile: conta=... chassi=Vitaspark classe=Blaster HP=23 CA=10 corpo=...
```

## 7. Pendências que podem travar

| # | Item | Dono |
|---|---|---|
| 1 | **Permissão de escrita fora do workspace** para o UnrealBuildTool (`~/Library/Application Support/Epic/`) | autor — sem isso não há build headless |
| 2 | Abertura do **editor** para migrar os widgets do canônico (saída A) | autor |
| 3 | ~~Decisão **login local × EOS**~~ — **resolvido**: EOS migrado, local como reserva (§5) | ✅ |
| 3b | **Credencial do Epic Dev Auth Tool** para o teste de EOS em execução | autor |
| 3c | Se o fluxo um dia exigir `ClientSecret` (troca de código / EOS Connect), ele vai para `Config/UserEngine.ini`, **não** para o versionado | autor |
| 4 | Nome das telas: manter `WBP_Login` ou renomear para o padrão do playbook (`WBP_Auth_Login`) | autor |
