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
| **EOS — Dev Auth** | `AuthType="developer"`, **Id = `localhost:<porta>` do tool**, **Token = o nome da credencial** criada nele (ver §5b) | testar o login real sem abrir o navegador (o caminho de DEV do autor) |
| **EOS — Conta Epic** | `AuthType="accountportal"`, sem Id/Token — o engine abre o portal da Epic | validar o caminho de produção |
| **Local (reserva)** | `Saved/RunnerAccounts.tsv`, hash MD5 (grau de desenvolvimento) | quando o EOS não está disponível; mantém o slice jogável offline |

O botão de EOS só aparece quando `IsEOSAvailable()` é verdadeiro (plugin carregado **e** interface de identidade obtida);
sem EOS, o widget mantém a porta local visível.

### 5a. O **ClientSecret** é obrigatório (e onde ele mora)

**Correção de rota, com prova:** eu tinha deixado o `ClientSecret` de fora achando que o Dev Auth não precisaria
dele. **Precisa.** Sem o segredo o EOS nem inicializa — e a falha é silenciosa para o jogador:

```
LogEOSSDK: Error: LogEOS: ClientCredentials.ClientSecret cannot be null
LogEOSSDK: Error: LogEOS: Invalid input platform options. EOS_EResult: EOS_NotConfigured
LogEOSShared: Warning: CreatePlatform failed, EosPlatformHandle=nullptr
LogOnline: Error: EOS: FOnlineSubsystemEOS::PlatformCreate() failed to init EOS platform
```

O `EOS_Platform_Create` valida **ClientCredentials completas** (ClientId **e** ClientSecret). É o modelo de
"cliente confidencial" do EAS — o mesmo que o canônico usa.

**A divisão que resolve os dois lados** (DEV funcionando sem segredo no Git):

| O que | Onde | Versionado? |
|---|---|---|
| identificadores públicos (ClientId, ProductId, SandboxId, DeploymentId, EncryptionKey), `DefaultArtifactName`, escopos | `Config/DefaultEngine.ini` | ✅ sim |
| **ClientSecret** | `Config/<Plataforma>/<Plataforma>Engine.ini` (ex.: `Config/Mac/MacEngine.ini`) | ❌ **no .gitignore** |

O engine lê o config de plataforma **depois** do `DefaultEngine.ini`, e lá a linha do artefato **sem o prefixo `+`
substitui a lista inteira** — é isso que permite trocar o artefato por um completo, com o segredo, sem tocar no
arquivo versionado.

**Para gerar:**

```sh
EOS_CLIENT_SECRET=<o segredo> python3 Tools/unreal/setup_eos_secret.py
```

O script lê os identificadores do `DefaultEngine.ini`, monta o artefato completo e grava no config de plataforma
— **nunca imprime o segredo** e nunca escreve em arquivo versionado.

> Por que não em `Saved/Config/<Plataforma>/Engine.ini`? Porque **o editor reescreve essa pasta ao fechar** e o
> arquivo some (aconteceu aqui: o EOS subiu numa execução e falhou na seguinte). O config de plataforma do projeto
> sobrevive.

> **Vigia:** a suíte `Runner.Sessao.EOSConfigurado` falha com a instrução do conserto se o EOS não subir, em vez de
> deixar o login cair para o local em silêncio.

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
2. Escolher uma **porta TCP** para o tool ouvir os pedidos de login (no nosso DEV: **`8081`**).
3. Entrar com a **conta Epic Games de desenvolvedor** (e-mail, senha e MFA) — dentro do tool.
4. Dar um **nome** à credencial (ex.: `dev-volt`). **Uma credencial por conta Epic**: para testar
   multiplayer local são necessárias duas contas Epic.
5. No menu do jogo, no passo de entrada: **campo 1** = `localhost:8081` · **campo 2** = `dev-volt`.

**Por que esses dois campos?** É o que a doc oficial define para o tipo `EOS_LCT_Developer` (e é o que o
menu agora mostra como rótulo, em vez de "conta/senha"):

| Campo do menu | `EOS_Auth_Credentials` | Valor |
|---|---|---|
| campo 1 | `Id` | `localhost:<porta>` — host e porta onde o tool está ouvindo |
| campo 2 | `Token` | o **nome** dado à credencial no tool |
| (botão) | `Type` | `developer` |

> O tool **precisa ficar rodando** enquanto o jogo faz o login. Se ele estiver fechado, o `EOS_Auth_Login`
> falha e o motivo aparece no próprio status do menu e no `LogOnline`.

**Estado do ambiente DEV (13/set, verificado):** host `localhost:8081` · credencial `dev-volt` · `lsof` confirma
`TCP *:8081 (LISTEN)` e o endpoint responde. Ou seja: **do lado do tool está pronto**; falta só o Play.

**Se o campo vier vazio** (foi o que aconteceu no primeiro Play), o SDK reclama e o log diz exatamente por quê:

```
LogEOSSDK: Error: LogEOSAuth: Invalid parameter EOS_Auth_Credentials.Id reason: must not be null or empty
LogOnline: Warning: EOS: Login(0) failed with EOS result code (EOS_InvalidParameters)
```

Dois consertos, os dois feitos: o campo 1 agora **já vem preenchido** com o host de
*Project Settings > Game > Runner > **EOS** > EOSDevAuthHost* (padrão `localhost:8081`) e o
`URunnerSession::LoginWithEOS` **recusa** o login incompleto com uma mensagem que diz o que digitar, em vez de
deixar o erro cru do SDK chegar ao jogador.

## 5b.1 Se o tool **não abrir** no macOS (aconteceu aqui)

São **três** sintomas encadeados, todos do macOS — nada a ver com o EOS:

| O que aparece | Por que |
|---|---|
| `"EOS_DevAuthTool" Not Opened — Apple cannot verify that "EOS_DevAuthTool" is free of malware` (com *Move to Trash*) | o app foi **baixado** e ficou com o atributo de **quarentena**. A assinatura é legítima (`codesign --verify` = *valid on disk*, Developer ID da Epic) — o Gatekeeper é que barra |
| Abre e mostra `A JavaScript error occurred in the main process / Error: ENOENT: no such file or directory` | o macOS rodou o app **translocado**, de `/private/var/folders/.../AppTranslocation/...` (caminho aleatório e somente-leitura). O app procura os próprios arquivos no caminho original e não acha |
| `"EOS_DevAuthTool" would like to access files in your Documents folder` | o app está dentro de `~/Documents`, que é pasta protegida. **Clique OK** (uma vez) |

**Conserto (uma vez):**

```sh
# 1. tirar a quarentena (o translocado e o bloqueio do Gatekeeper acabam juntos)
xattr -dr com.apple.quarantine "/caminho/SDK/Tools/EOS_DevAuthTool.app"

# 2. abrir pelo caminho real
open "/caminho/SDK/Tools/EOS_DevAuthTool.app"

# 3. conferir que NAO esta mais translocado (o caminho tem que ser o real)
pgrep -fl EOS_DevAuthTool
```

Se o `xattr` responder `Operation not permitted`: quem está rodando o comando não tem permissão de escrita fora do
projeto — rode você mesmo no Terminal, ou autorize o modo amplo.

## 5c. Regras de conta (a tela de **criar conta**)

Pedido do autor: os campos obrigatórios e a regra da senha. A regra vive em **C++**, numa função pura
(`URunnerSession::ValidateNewAccount`) — a tela só mostra o texto que ela devolve, e o teste cobre caso a caso
(`Runner.Sessao.RegrasDeConta`).

| Campo | Regra | Mensagem quando falha |
|---|---|---|
| **E-mail** | um `@`, sem espaços, com domínio de ponto | *Informe um e-mail valido (ex.: nome@dominio.com).* |
| **Nome de usuário** | 3+ caracteres, só letras, números, `_` ou `-` | *O nome de usuario precisa de pelo menos 3 caracteres.* |
| **Senha** | 8+ caracteres, com **maiúscula**, **minúscula** e **caractere especial** | *A senha precisa de pelo menos uma letra maiuscula.* (idem para minúscula e especial) |
| **Confirmar senha** | igual à senha | *A confirmacao precisa ser igual a senha.* |

**Modelo da conta local:** o **e-mail é a identidade** (a chave em `Saved/RunnerAccounts.tsv`) e o **nome de usuário
é o que aparece na tela**. O arquivo passou a ter três colunas (`email ⇥ hash ⇥ nome de usuario`); contas antigas
de duas colunas continuam sendo lidas. Entrar é sempre pelo e-mail; personagens salvos passam a ser
chaveados pela identidade da conta (e-mail no local, conta EOS na plataforma).

> **Limite honesto:** não há verificação de e-mail nem recuperação de senha, e o hash é MD5 (grau de
> desenvolvimento). Quem valida identidade de verdade é o EOS/EAS — a conta local existe para o slice rodar
> offline. Registrado no backlog como `MIG-010`.

## 5d. Cara de jogo: paleta, nome do personagem e o véu

### O nome do personagem

A tela de criação passou a pedir o **nome do Runner** (no passo da classe, junto do botão de criar). A regra é
pura e testada (`URunnerSession::ValidateCharacterName`, caso a caso em `Runner.Sessao.RegrasDeConta`):

| Regra | Mensagem |
|---|---|
| obrigatório | *De um nome ao seu Runner.* |
| 3 a 16 caracteres | *O nome do Runner precisa de pelo menos 3 caracteres.* (idem máximo) |
| letras, números, espaço, `_` ou `-` | *No nome do Runner use letras, numeros, espaco, _ ou -.* |
| único dentro da conta | *Ja existe um Runner chamado 'Volt' nesta conta.* |

O nome viaja junto: fica na ficha (`FRunnerCharacterProfile::CharacterName`) e é ele que aparece na lista de
Runners (`FRunnerSavedCharacter::GetDisplayName()`), com *chassi · classe* só como reserva para personagens
gravados antes do campo existir.

### A paleta

`Source/PloidrekRPG/Public/UI/RunnerPalette.h` traduz o documento canônico `Docs/SteampunkPalette.md` para código:
latão `#c9a227`, âmbar `#ffb040`, ferro `#3a3a42`, aço `#2a2a30`, silhueta `#1a1a2e`, rebites `#d8c890`,
vapor `#c8c0a8`, forja `#c86a30`. **Nenhum hexadecimal solto no widget** — quem quiser uma cor nova, primeiro
ela existe na paleta de arte.

O que mudou na tela: painel de aço escuro com contorno de latão (caixa arredondada, sem precisar de textura),
cabeçalho com a marca + o passo atual e um filete de metal, botão principal âmbar com texto escuro, secundário
em ferro, terceiro em cobre, campos escuros que acendem no foco, e respiro entre os elementos.

### O véu

`URunnerVeilWidget` é um efeito de **interface** em C++/Slate — não é material nem asset: o gradiente radial é
gerado em memória (`UTexture2D::CreateTransient`, queda `(1-d)^2.2`) e pintado no `NativePaint`.

| O que | Como |
|---|---|
| circulam pela tela | fios de neblina vapor em curvas de Lissajous com fases no ângulo áureo |
| rondam os botões | cada botão/campo do passo vira um alvo; o fio orbita com raio **maior** que o retângulo, então a neblina **passa por cima** do botão e não lê como fundo |
| seguem o mouse | a bola de dispersão: halo largo + neblina cheia + calor de forja no centro, sempre no cursor |

O véu é o **último filho do canvas**, e é isso que faz ele desenhar por cima do painel. Ele é
`HitTestInvisible`, então **não rouba clique** de nenhum botão.

**Ajustar sem recompilar** — *Project Settings > Game > Runner > **Veu***:

| Campo | Para que |
|---|---|
| `bVeuLigado` | liga/desliga |
| `IntensidadeDoVeu` | 0 quase invisível · 1 como desenhado · 1.5 carregado |
| `RaioDaBolaDoMouse` | tamanho da bola que segue o cursor (padrão 300 px) |
| `VelocidadeDoVeu` | velocidade da circulação |

Em execução: console **`Runner.Veil 0`** desliga na hora (e `1` liga).

> **Como conferir sem o jogo:** `python3 Tools/preview/veil_preview.py Saved/Preview_Veu.png` desenha a mesma
> composição (mesmos números, mesmo gradiente) num PNG. Foi assim que a bola do mouse saiu de discreta para
> cheia: a primeira calibragem lia como névoa, não como bola.

## 5e. Vitrine 3D, ficha na tela, confirmação e exclusão

### A vitrine (o corpo parado enquanto você escolhe)

`ARunnerPreviewActor` (`Source/PloidrekRPG/Public/UI/RunnerPreviewActor.h`) é um ator que nasce **longe do mapa**
(Z = 50.000: ninguém vê o boneco no cenário) com:

| Peça | Papel |
|---|---|
| `USkeletalMeshComponent` | o corpo do chassi, girando devagar (16°/s) para mostrar o volume |
| `USceneCaptureComponent2D` | câmera apontada para o corpo, escrevendo numa `UTextureRenderTarget2D` de 512² |
| `UDirectionalLightComponent` (0.6, ciano) | só preenchimento: a luz do mundo já ilumina, isto evita o lado chapado |

A captura usa `PrimitiveRenderMode = UseShowOnlyList` com **só este ator** na lista: o mapa do menu não entra na
imagem, mas a luz do mundo (sol e skylight) continua valendo. A tinta do chassi entra pelo **mesmo material de
overlay do jogo** (parâmetro `AccentColor`), então a cor na vitrine é a cor que o pawn vai ter em campo.

O widget mostra isso num **monitor**: moldura de aço com filete de latão (`230 × 300`). Passear o mouse pela lista
já troca o corpo (`OnHovered` da linha) — é assim que dá para comparar chassi sem clicar.

> ⚠️ **O que a automação não alcança:** a captura só existe com o renderizador ligado, então nenhum teste headless
> vê essa imagem. O código é verificado por compilação e pelo resto da suíte; **o visual é o seu Play que julga**.
> Se o corpo aparecer de cabeça para baixo, é o flip do alvo de captura — um ajuste de uma linha no pincel.

### A ficha (janela de atributos)

Ao lado do monitor, a ficha do que está selecionado — tudo vindo dos dados e das regras, nada escrito à mão:

- **nome, corpo e tinta** do chassi;
- os **seis atributos** com valor final (base 8 + bônus do chassi) e modificador — bônus positivo em Aether, negativo em cobre;
- **vantagens** primária e secundária;
- com a classe escolhida: **HP nível 1, CA, dado de vida, foco (atributos primário/secundário) e células de Éter**, tudo calculado por `URunnerRules`.

### Confirmação e exclusão

Nada de criar ou apagar em um clique só. O passo `Confirm` é uma janela:

| Ação | O que a janela mostra | Confirmar faz |
|---|---|---|
| **Criar personagem** | nome, corpo (com o tipo), classe, HP nível 1, CA e o dado de vida | cria a ficha e entra no jogo |
| **Excluir** (botão ao lado de cada Runner) | o nome e chassi · classe do Runner, avisando que não tem volta | apaga, e a lista se refaz |

O nome é validado **antes** de abrir a janela (erro de nome aparece direto, sem confirmação inútil). Cancelar volta
para o passo que pediu a confirmação.

`URunnerSession::DeleteSavedCharacter` só apaga personagem **da conta logada**; se era o que estava em uso, a ficha
ativa sai junto. Personagens passam a usar o **primeiro id livre** (`R-01`, `R-02`, …), então excluir alguém do meio
não faz dois Runners com o mesmo id — e o nome do apagado volta a ficar disponível.

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
| **10** ✅ | **feito:** nome do personagem (regra + ficha + lista), paleta canônica no código (`RunnerPalette.h`), menu com cara de jogo e o **véu** que circula pela tela, ronda os botões e se abre em bola no mouse | **9 suítes verdes**; véu calibrável em *Project Settings > Game > Runner > Veu* e desligável com `Runner.Veil 0` |
| **11** ✅ | **feito:** vitrine 3D do chassi (captura para textura, mostrada num "monitor"), ficha de atributos ao lado, **confirmação** ao criar e **excluir com confirmação** | **9 suítes verdes** (a `VariosPersonagens` cobre apagar, id reaproveitado e nome liberado) |
| **12 (atual)** | **teste de Play do autor** com vitrine, ficha, véu e as confirmações | play mostra o corpo girando no monitor, a ficha mudando ao passar o mouse, e as duas janelas de confirmação |
| **13** | migração dos widgets do canônico (saída A) e, se o autor quiser, WBP por cima do C++ | menu com arte própria |
| **12** | migração dos widgets do canônico (saída A) e, se o autor quiser, WBP por cima do C++ | menu com arte própria |

## 6b. Como o fluxo roda hoje (passo a passo)

```
1. Abrir o projeto e dar Play em NewMap
   └ o WorldSettings do mapa aponta para BP_MenuGameMode (pai: AMenuGameMode)
     └ abre URUNNERMENUWIDGET: tela de login
2. Entrar: "Entrar com o EOS (Dev Auth Tool)"  [LoginWithEOS("developer", host:porta, credencial)]
   └ o campo 1 ja vem preenchido com EOSDevAuthHost (Project Settings > Game > Runner > EOS)
   └ a Auth Interface do EOS responde → OnLoginComplete → a sessão toma o nome da conta EOS
   └ sem EOS: "Criar conta nova (local)" → formulário com e-mail, nome de usuario, senha e
     confirmacao, validado por ValidateNewAccount (ver §5c)   [CreateAccount / Login]
3. JANELA PÓS-LOGIN: "escolha seu Runner"
   ├── conta COM personagens → lista (R-01, R-02, ...) → jogar com o escolhido  [SelectSavedCharacter]
   │     └ pula direto para o passo 6 (chassi e classe vêm da ficha salva)
   └── conta SEM personagens → "Criar novo Runner" → segue para o passo 4
3b. Escolher o chassi  (10 opções + os Clyffen aparecem como extintos e são recusados)
4. Escolher a classe  (6 opções)
5. Dar o NOME do Runner e criar                [ValidateCharacterName -> Confirm -> CreateCharacterProfile]
   └ regra: 3 a 16 caracteres, letras/numeros/espaco/_/-, unico na conta
   └ a janela de confirmacao mostra corpo, classe, HP, CA e o dado de vida antes de valer
   └ na lista de Runners, o botao Excluir (ao lado de cada um) tambem passa pela confirmacao
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
