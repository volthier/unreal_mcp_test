# =====================================================================
# AETHER FORGE X — PROTOCOL ZERO
# MASTER IMPLEMENTATION PROMPT
# CHARACTER CREATION — STEP 1: ESSENCE / CHASSIS
#
# TARGET:
# TCPC_Tela_Criacao_Personagem_Chassi_art_final.png
#
# SUPPORTING SPECIFICATION SHEETS:
# TCPC_guia_conceitual_part1.png
# TCPC_guia_conceitual_part2.png
# TCPC_guia_conceitual_part3.png
#
# ENGINE / TOOLS:
# Unreal Engine 5.8
# Unreal MCP
# Blender MCP
# ComfyUI
# UMG / CommonUI when already adopted by project
# Niagara
# Material Editor
# Lumen
# Enhanced Input when already adopted
# =====================================================================


Você é o agente técnico, artístico e de arquitetura responsável por implementar
dentro do projeto existente AETHER FORGE X — PROTOCOL ZERO a PRIMEIRA ETAPA da
CRIAÇÃO DE PERSONAGEM:

1. ESSÊNCIA / CHASSI
2. APARÊNCIA / CORPO
3. CONFIRMAÇÃO

ESTA TAREFA IMPLEMENTA PRINCIPALMENTE A ETAPA:

1 — ESSÊNCIA (CHASSI)

e deve preparar corretamente a navegação e os contratos para as etapas seguintes,
SEM tentar resolver arbitrariamente toda a criação de personagem em um único
widget.

O resultado final NÃO é uma imagem.

O resultado final deve ser uma tela REAL, FUNCIONAL, DATA-DRIVEN, ANIMADA,
RESPONSIVA e INTEGRADA ao restante do sistema no Unreal Engine 5.8.


# =====================================================================
# 0. PRIORIDADE DAS REFERÊNCIAS
# =====================================================================

Existem quatro referências.

PRIORIDADE 1 — TARGET VISUAL CANÔNICO:

TCPC_Tela_Criacao_Personagem_Chassi_art_final.png

Esta imagem representa A TELA QUE QUEREMOS OBTER.

Ela possui aproximadamente:

1536 × 1024

e define:

- composição;
- proporções;
- posição dos elementos;
- hierarquia;
- framing;
- cenário;
- distribuição dos cristais;
- tamanho dos painéis;
- cores;
- intensidade de glow;
- aparência de Forgekin selecionado;
- títulos;
- textos;
- botões;
- aparência geral.

PRIORIDADE 2 — DOCUMENTAÇÃO:

TCPC_guia_conceitual_part1.png
TCPC_guia_conceitual_part2.png
TCPC_guia_conceitual_part3.png

Essas três imagens são SPECIFICATION SHEETS.

Elas explicam:

- como construir a tela;
- biblioteca dos cristais;
- materiais;
- shaders;
- VFX;
- Blender;
- ComfyUI;
- Unreal;
- layout;
- estados;
- cores;
- atributos;
- habilidades;
- arquitetura visual.

REGRA:

SE HOUVER QUALQUER CONFLITO VISUAL ENTRE AS PRANCHAS E A ARTE FINAL:

TCPC_Tela_Criacao_Personagem_Chassi_art_final.png

SEMPRE TEM PRIORIDADE.

NÃO reproduza dentro do jogo:

- caixas explicativas das pranchas;
- números das seções;
- callouts;
- setas explicativas;
- notas de produção;
- miniaturas de referência;
- legendas técnicas.

As pranchas explicam COMO construir o target.

A arte final mostra O QUE deve aparecer para o jogador.


# =====================================================================
# 1. CONTEXTO DO SISTEMA JÁ EXISTENTE
# =====================================================================

ANTES DE ALTERAR QUALQUER COISA:

INSPECIONE O PROJETO EXISTENTE.

Já foram ou estão sendo criadas telas anteriores:

LOGIN

↓

SELEÇÃO DE PERSONAGEM

↓

CRIAÇÃO DE PERSONAGEM

Portanto NÃO crie três sistemas independentes.

A arquitetura deve formar um fluxo coerente:

Login
   ↓
Authenticated Session
   ↓
Character Selection
   ↓
Player selects "Criar Novo"
   ↓
Character Creation
   ↓
Step 1 — Essence / Chassis
   ↓
Step 2 — Appearance / Body
   ↓
Step 3 — Confirmation
   ↓
Persist Character
   ↓
Character Created
   ↓
Return to Character Selection
   ↓
New character appears in player's account
   ↓
Player can Enter World


# =====================================================================
# 2. REGRA CRÍTICA DO CHARACTER CREATION
# =====================================================================

UM PERSONAGEM AINDA NÃO EXISTE NA CONTA DURANTE A EDIÇÃO DO DRAFT.

Ao iniciar criação:

NÃO adicionar personagem à lista real da conta.

Criar apenas:

CharacterCreationDraft

O personagem só passa a existir realmente quando:

Step 3 — Confirmation
        ↓
Validation Success
        ↓
CreateCharacter Use Case
        ↓
Backend / Save persistence succeeds
        ↓
CharacterCreated domain/application event

SOMENTE DEPOIS DISSO:

Character Selection deve atualizar e mostrar o novo personagem.

Isso preserva a regra da tela anterior:

VISIBLE CHARACTERS =
CHARACTERS ACTUALLY OWNED BY ACCOUNT

Um CharacterCreationDraft NÃO conta como personagem existente.


# =====================================================================
# 3. ACCOUNT SLOT VALIDATION
# =====================================================================

A tela Character Creation somente pode ser aberta se a conta puder criar um
personagem.

NÃO confiar apenas no botão da UI anterior.

Validar também na camada Application/Domain.

Exemplo:

CanCreateCharacter(AccountId)

deve verificar:

CurrentCharacters < MaxCharacterSlots

Se não houver slot:

StartCharacterCreation deve falhar de forma controlada.

Não permitir bypass navegando diretamente para a tela.


# =====================================================================
# 4. ARQUITETURA — DDD
# =====================================================================

Aplicar Domain-Driven Design sem transformar Unreal em uma arquitetura
desnecessariamente burocrática.

Separar claramente responsabilidades.

BOUNDED CONTEXTS / DOMAINS CONCEITUAIS:

IDENTITY / ACCESS

Responsável por:

- login;
- authenticated session;
- AccountId;
- PlayerId;
- authentication tokens;
- current user.

Já existe a partir da tela de Login.

Character Creation NÃO deve conhecer WBP_LoginScreen.


CHARACTER ROSTER / CHARACTER MANAGEMENT

Responsável por:

- personagens pertencentes à conta;
- quantidade de slots;
- character selection;
- delete character;
- enter world;
- refresh roster.

Já existe conceitualmente a partir da tela Character Selection.

Character Creation NÃO deve manipular diretamente o widget do roster.


CHARACTER CREATION

Responsável por:

- criação de draft;
- escolha de chassis;
- escolha de aparência;
- confirmação;
- validação;
- criação definitiva do personagem.

Este é o domínio principal desta tarefa.


CHASSIS CATALOG

Responsável por:

- definições disponíveis de chassis;
- atributos base;
- afinidades;
- habilidades;
- dificuldade;
- visual;
- material;
- VFX;
- availability;
- gameplay tags.


# =====================================================================
# 5. CHARACTER CREATION AGGREGATE
# =====================================================================

Criar um modelo equivalente a:

CharacterCreationDraft

Possíveis propriedades:

DraftId
AccountId
SelectedChassisId
AppearanceData
CustomizationData
CharacterName
CurrentStep
CreatedAt
LastModifiedAt

No Step 1:

SelectedChassisId

é o principal dado sendo editado.

A seleção NÃO deve ser mantida apenas dentro do widget.

O widget apresenta o estado.

O Application Layer altera o draft.


# =====================================================================
# 6. VALUE OBJECTS / DOMAIN TYPES
# =====================================================================

Evite primitive obsession quando fizer sentido.

Exemplos conceituais:

ChassisId
AccountId
CharacterId
CharacterLevel
CharacterSlotCount
EtherAffinity
Difficulty
PloidrexAttributes
AbilityDefinition
CharacterCreationStep

Gameplay Tags também podem ser utilizados para classificação:

Chassis.Charger
Chassis.Vitaspark
Chassis.Aetheric
Chassis.Techno
Chassis.Droneframe
Chassis.Forgekin
Chassis.Ghostnet
Chassis.Vox
Chassis.Overcore
Chassis.Cryonix
Chassis.Clyffen

Role.Impact
Role.Speed
Role.Harmony
Role.Technology
Role.Support
Role.Defense
Role.Infiltration
Role.Presence
Role.Power
Role.Control

Status.Available
Status.Locked
Status.Extinct


# =====================================================================
# 7. APPLICATION USE CASES
# =====================================================================

Criar ou reutilizar casos de uso equivalentes a:

StartCharacterCreation
GetCharacterCreationDraft
GetAvailableChassis
SelectChassis
PreviewExampleBody
ContinueToAppearance
ReturnToPreviousCreationStep
CancelCharacterCreation
ConfirmCharacterCreation

Não coloque essas decisões diretamente dentro de Button OnClicked.


# =====================================================================
# 8. REPOSITORIES / PORTS
# =====================================================================

Criar interfaces quando necessárias:

ICharacterCreationRepository
IChassisCatalogRepository
ICharacterRosterRepository
IAccountRepository

A implementação concreta pode consumir:

PrimaryDataAssets;
SaveGame;
backend;
database;
HTTP service;
local mocks;

sem alterar o domínio ou os widgets.


# =====================================================================
# 9. GOF / DESIGN PATTERNS
# =====================================================================

UTILIZE PADRÕES SOMENTE QUANDO ELES RESOLVEREM UM PROBLEMA REAL.

NÃO implemente os 23 padrões GoF apenas para dizer que existem.

STATE PATTERN / STATE MACHINE

É apropriado para:

Essence
Appearance
Confirmation

Estados:

CharacterCreation.Essence
CharacterCreation.Appearance
CharacterCreation.Confirmation

A transição deve ser explicitamente validada.


STRATEGY

Pode ser utilizado para comportamentos realmente variáveis, por exemplo:

IChassisAvailabilityPolicy
IChassisPreviewStrategy

Não utilizar Strategy onde simples data configuration resolve.


FACTORY

Pode ser utilizada para:

ChassisPreviewActorFactory

quando diferentes chassis necessitarem actors ou componentes diferentes.

Não criar `switch Forgekin / switch Vox / switch Techno` espalhados pelo projeto.


OBSERVER

Preferir:

Unreal delegates;
Event Dispatchers;
Field Notify;
MVVM se o projeto já utilizar;

para propagar mudança de estado.

A UI deve reagir a:

SelectedChassisChanged
CreationStepChanged
ChassisAvailabilityChanged


COMMAND

Input actions podem ser representadas como comandos claros:

Select
Back
Continue
PreviewBody
MoveLeft
MoveRight


REPOSITORY

Aplicar como padrão DDD para acesso a dados.


# =====================================================================
# 10. SOLID / CLEAN CODE
# =====================================================================

Aplicar:

SRP
OCP
LSP quando houver polimorfismo
ISP
DIP

Especialmente:

Widget NÃO chama diretamente backend.

Widget NÃO contém regras de slot.

Widget NÃO determina quais chassis são permitidos.

Widget NÃO grava personagem.

Widget NÃO implementa lógica de domínio.

Widget apresenta estado e envia intenções.


EVITAR:

God Blueprint;
God Widget;
500-node Event Graph;
Cast chains;
GetAllActorsOfClass usado como arquitetura;
hardcoded FString;
hardcoded colors;
hardcoded chassis em widgets;
Tick para animação simples;
duplicação de materiais;
duplicação de botões;
duplicação de estilos.


# =====================================================================
# 11. DATA-DRIVEN CHASSIS CATALOG
# =====================================================================

Os chassis NÃO devem ser codificados individualmente dentro do UMG.

Criar definição data-driven.

Preferência:

PrimaryDataAsset

por chassis.

Exemplo:

PDA_Chassis_Charger
PDA_Chassis_Vitaspark
PDA_Chassis_Aetheric
PDA_Chassis_Techno
PDA_Chassis_Droneframe
PDA_Chassis_Forgekin
PDA_Chassis_Ghostnet
PDA_Chassis_Vox
PDA_Chassis_Overcore
PDA_Chassis_Cryonix
PDA_Chassis_Clyffen

Estrutura equivalente:

FChassisDefinition

com:

ChassisId
DisplayName
ArchetypeLabel
GemName
ShortDescription
Lore
Quote
BodyTypeHint
EtherAffinity
Difficulty
BaseAttributes
PrimaryAbility
SecondaryAbility
GameplayStyleTags
PrimaryColor
SecondaryColor
CrystalMesh
CrystalMaterial
CrystalMaterialInstance
IdleVFX
SelectedVFX
ClassIcon
PreviewBodyActor
AvailabilityState
SortOrder

Usar:

SoftObjectPtr / soft references

quando apropriado.

Integrar ao Unreal Asset Manager se a arquitetura existente utilizar.


# =====================================================================
# 12. NÃO INVENTAR GAME DESIGN
# =====================================================================

As imagens fornecem valores completos principalmente para FORGEKIN.

NÃO invente atributos ou habilidades dos outros chassis caso esses dados ainda
não existam no projeto.

Para os outros chassis:

utilize os dados oficiais existentes no projeto.

Se ainda não existirem:

crie a estrutura e deixe dados específicos claramente configuráveis.

NÃO transformar suposição artística em regra definitiva de gameplay.


# =====================================================================
# 13. CHASSIS LIBRARY CANÔNICA
# =====================================================================

A referência define:

10 CHASSIS JOGÁVEIS
+
1 CHASSIS EXTINTO / BLOQUEADO

Ordem visual canônica:

1. CHARGER
2. VITASPARK
3. AETHERIC
4. TECHNO
5. DRONEFRAME
6. FORGEKIN
7. GHOSTNET
8. VOX
9. OVERCORE
10. CRYONIX
11. CLYFFEN


# =====================================================================
# 14. CHARGER
# =====================================================================

ID:

Charger

DISPLAY:

CHARGER

IDENTIDADE:

IMPACTO

Descrição:

Força bruta e avanço implacável.

Cristal:

Rubi Bruto.

Formato:

grande shard rubi;
facetas agressivas;
alongado;
pontas afiadas.

Cor canônica:

#FF2B2B

Energia:

vermelho;
faíscas;
pequenos fragmentos.


# =====================================================================
# 15. VITASPARK
# =====================================================================

DISPLAY:

VITASPARK

IDENTIDADE:

VELOCIDADE

Descrição:

Movimento e precisão.

Cristal:

Estrela Solar.

Formato:

starburst cristalino;
múltiplas pontas;
estrutura radial.

Cor:

#FFC41A

Energia:

amarelo-dourada;
pulsos rápidos;
spark energy.


# =====================================================================
# 16. AETHERIC
# =====================================================================

DISPLAY:

AETHERIC

IDENTIDADE:

HARMONIA

Descrição:

Suporte e canalização de Éter.

Cristal:

Orbe Atômico.

Formato:

núcleo/orbe cristalino;
anéis de energia orbitando.

Cor:

#00E088

Energia:

verde-esmeralda;
orbital;
harmônica.


# =====================================================================
# 17. TECHNO
# =====================================================================

DISPLAY:

TECHNO

IDENTIDADE:

TECNOLOGIA

Descrição:

Controle e engenharia.

Cristal:

estrutura cúbica azul.

Formato:

cubo tecnológico;
geometria extremamente regular;
linhas internas.

Cor:

#00B4FF

Energia:

azul;
grid;
digital;
precisa.


# =====================================================================
# 18. DRONEFRAME
# =====================================================================

DISPLAY:

DRONEFRAME

IDENTIDADE:

SUPORTE

Descrição:

Drones e reconhecimento.

Cristal:

estrutura piramidal/jade.

Formato:

cristal verde vertical;
facetas complexas;
pontas múltiplas.

Cor:

#00D68F

Energia:

jade / teal.


# =====================================================================
# 19. FORGEKIN
# =====================================================================

DISPLAY:

FORGEKIN

IDENTIDADE:

DEFESA

Cristal:

Cristal Vulcânico.

Formato:

cluster irregular;
rocha cristalina;
múltiplas pontas;
aparência de magma solidificado.

Cor:

#FF8A00

IMPORTANTE:

FORGEKIN é o chassis selecionado na arte final utilizada como target visual.

Isso NÃO significa que runtime deve sempre iniciar com Forgekin.

Para testes de screenshot / visual regression:

utilize um fixture que selecione Forgekin.

Em runtime:

se o draft já possuir chassis:

restaurar a seleção do draft.

Caso contrário:

obedecer regra de UX configurada pelo projeto.


# =====================================================================
# 20. GHOSTNET
# =====================================================================

DISPLAY:

GHOSTNET

IDENTIDADE:

INFILTRAÇÃO

Descrição:

Ocultação e manipulação.

Cristal:

Obelisco Fragmentado.

Cor:

#8B40FF

Formato:

vertical;
violeta;
fragmentado;
sharp.


# =====================================================================
# 21. VOX
# =====================================================================

DISPLAY:

VOX

IDENTIDADE:

PRESENÇA

Descrição:

Som e influência.

Cristal:

Anel Ressonante.

Cor:

#FF3ED1

Formato:

anel / loop energético;
núcleo vazio;
energia resonante.

VFX:

ondas;
pulsos;
resonance rings.


# =====================================================================
# 22. OVERCORE
# =====================================================================

DISPLAY:

OVERCORE

IDENTIDADE:

PODER

Descrição:

Sobrecarga e risco.

Cristal:

Cristal Ascendente.

Cor:

#FFFFFF

Formato:

shard branco vertical;
energia orbital dourada/branca.

VFX:

alta intensidade;
órbitas;
overcharge.


# =====================================================================
# 23. CRYONIX
# =====================================================================

DISPLAY:

CRYONIX

IDENTIDADE:

CONTROLE

Descrição:

Gelo e lentidão.

Cristal:

Floco Hexagonal.

Cor:

#3BD6FF

Formato:

snowflake / crystal hexagonal;
simetria radial.

VFX:

ice-blue;
cold particles;
crystalline sparkle.


# =====================================================================
# 24. CLYFFEN
# =====================================================================

DISPLAY:

CLYFFEN

IDENTIDADE:

LENDÁRIO

Descrição:

Memória de um tempo perdido.

Cristal:

Cristal Rachado / Opala Fóssil.

Cor:

#B8863B

ESTADO ATUAL:

EXTINTO / BLOQUEADO.

Na arte:

dark amber crystal;
baixa luminosidade;
lock icon.

Clyffen aparece na biblioteca porém:

NÃO pode ser selecionado.

A disponibilidade NÃO deve estar codificada diretamente no widget.

Usar:

AvailabilityState
ou
IChassisAvailabilityPolicy.

Assim o chassis poderá futuramente ser desbloqueado sem redesenhar a UI.


# =====================================================================
# 25. PALETA DE CRISTAIS
# =====================================================================

CHARGER
#FF2B2B

VITASPARK
#FFC41A

AETHERIC
#00E088

TECHNO
#00B4FF

DRONEFRAME
#00D68F

FORGEKIN
#FF8A00

GHOSTNET
#8B40FF

VOX
#FF3ED1

OVERCORE
#FFFFFF

CRYONIX
#3BD6FF

CLYFFEN
#B8863B


# =====================================================================
# 26. UI COLOR SYSTEM — CHARACTER CREATION
# =====================================================================

Utilizar o Design System já iniciado nas telas Login e Character Selection.

NÃO criar outra linguagem visual.

Tokens desta tela:

UI PRIMARY
#00D4FF

UI SECONDARY
#FF2D9B

ACCENT ORANGE
#FF9A3C

BACKGROUND 1
#0A0F1A

BACKGROUND 2
#111827

PRIMARY TEXT
#E6F1FF

SUCCESS
#22C55E

WARNING
#FACC15

ERROR
#EF4444

Globalmente também preservar linguagem existente:

cyan;
magenta;
violeta;
dark navy;
gunmetal;
white.


# =====================================================================
# 27. SHARED DESIGN SYSTEM
# =====================================================================

Antes de criar novos assets:

LOCALIZE o Design System criado para:

Login
Character Selection

Reutilize quando semanticamente correto:

logo;
fonts;
frames;
base button;
hover effects;
focus effects;
materials;
scanline;
glow;
noise;
cyber borders;
sound hooks;
input mappings.

NÃO copie arquivos e apenas troque o nome.

Fatorar componentes compartilhados quando necessário.

Exemplo conceitual:

/Game/UI/Common/

Theme/
Fonts/
Materials/
Widgets/
Icons/
Animations/

Componentes desta tela devem reutilizar essa base.


# =====================================================================
# 28. TIPOGRAFIA
# =====================================================================

Logo:

reutilizar logo oficial existente AETHER FORGE X — PROTOCOL ZERO.

NÃO recriar logo diferente.

Título principal:

Orbitron ou fonte equivalente já adotada.

UI:

Exo 2.

Suporte:

Noto Sans JP.

Se o projeto já tiver equivalentes aprovados:

reutilizá-los.

Não adicionar fonte quase idêntica desnecessariamente.


# =====================================================================
# 29. TARGET COMPOSITION LOCK
# =====================================================================

TCPC_Tela_Criacao_Personagem_Chassi_art_final.png

é o target de composição.

Preservar:

logo superior esquerdo;
título central superior;
stepper;
slogan superior direito;
texto introdutório esquerdo;
cidade Nexus-7 central;
linha horizontal de cristais;
Forgekin selecionado;
painel inferior de Forgekin;
painel de atributos;
painel de habilidades;
painel Próximo Passo;
footer.

NÃO transformar em wizard de tela branca.

NÃO transformar em modal.

NÃO transformar em menu vertical genérico.

NÃO transformar em dashboard.


# =====================================================================
# 30. BACKGROUND — NEXUS-7
# =====================================================================

A tela acontece em:

NEXUS-7.

Texto ambiental:

NEXUS-7

ALÉM DA NÉVOA,
UM NOVO AMANHÃ.

Reutilizar o ambiente criado para Login e Character Selection sempre que possível.

Idealmente existir um componente compartilhado equivalente a:

BP_Nexus7_MenuBackdrop

ou:

MenuWorld.Nexus7

com diferentes:

CameraProfile;
lighting;
foreground;
composition.

NÃO criar três cópias completas da mesma cidade para três telas.


# =====================================================================
# 31. BACKGROUND VISUAL
# =====================================================================

Nexus-7 deve possuir:

megacidade colossal;
torres extremamente verticais;
arquitetura dark sci-fi;
magenta emissive;
cyan accents;
purple storm;
fog;
volumetric clouds;
lightning;
distant drones;
floating structures;
nano swarms;
depth;
parallax.

Nesta tela:

os cristais são o foreground visual.

A cidade permanece monumental porém ligeiramente menos dominante que na
Character Selection screen.


# =====================================================================
# 32. TEXTOS SUPERIORES
# =====================================================================

CENTRO:

CRIAÇÃO DE PERSONAGEM

Abaixo:

1  ESSÊNCIA (CHASSI)
2  APARÊNCIA (CORPO)
3  CONFIRMAÇÃO

Step 1 deve estar ativo.

Step 2 e Step 3 inativos.

SUPERIOR DIREITO:

UM NÚCLEO. INFINITAS POSSIBILIDADES.

“ESCOLHA O QUE TE MOVE.”


# =====================================================================
# 33. WIZARD / STEPPER
# =====================================================================

Criar componente reutilizável:

WBP_CharacterCreationStepper

Steps:

1 ESSÊNCIA (CHASSI)
2 APARÊNCIA (CORPO)
3 CONFIRMAÇÃO

Estados:

Completed
Current
Available
Disabled

Nesta tela:

Essence = Current
Appearance = Available/Next
Confirmation = Future

Não permitir pular etapas quando o domínio não autorizar.


# =====================================================================
# 34. INTRODUÇÃO — ESQUERDA
# =====================================================================

Título:

ESCOLHA SUA ESSÊNCIA

Texto:

Cada cristal de Éter é um Chassi.
Ele define sua afinidade, atributos iniciais,
habilidades e sua relação com o Éter.
O corpo vem depois.

Quote:

“O cristal escolhe quem pode carregá-lo,
mas o que você se torna, é com você.”

O texto deve permanecer muito próximo da composição do target.


# =====================================================================
# 35. PRINCÍPIO NARRATIVO
# =====================================================================

ESSÊNCIA = CHASSI.

O jogador escolhe PRIMEIRO quem será mecanicamente.

A aparência vem DEPOIS.

O Chassi define:

- afinidade;
- atributos iniciais;
- abilities;
- gameplay identity;
- relação com Éter;
- tendência corporal inicial.

O corpo define:

forma visual.

A aparência NÃO deve alterar arbitrariamente as regras do chassis.


# =====================================================================
# 36. CRYSTAL SELECTION ROW
# =====================================================================

Mostrar os 11 chassis em uma grande linha/carousel:

CHARGER
VITASPARK
AETHERIC
TECHNO
DRONEFRAME
FORGEKIN
GHOSTNET
VOX
OVERCORE
CRYONIX
CLYFFEN

No target 1536×1024:

todos aparecem simultaneamente.

Em resoluções menores:

não destruir proporções para forçar tudo.

Utilizar:

horizontal carousel;
scroll;
selection centering;
left/right navigation.

Preservar boa legibilidade.


# =====================================================================
# 37. CRYSTALS DEVEM SER 3D
# =====================================================================

NÃO representar os cristais finais apenas por PNG.

Criar meshes 3D.

Usar Blender MCP.

Cada cristal deve possuir:

unique silhouette;
unique geometry;
unique material profile;
unique emissive color;
unique VFX personality.

Podem existir concept textures produzidas pelo ComfyUI, porém o elemento final
deve ser um objeto tridimensional sempre que tecnicamente adequado.


# =====================================================================
# 38. BLENDER CRYSTAL ASSETS
# =====================================================================

Criar:

SM_Chassis_Charger_Crystal
SM_Chassis_Vitaspark_Crystal
SM_Chassis_Aetheric_Crystal
SM_Chassis_Techno_Crystal
SM_Chassis_Droneframe_Crystal
SM_Chassis_Forgekin_Crystal
SM_Chassis_Ghostnet_Crystal
SM_Chassis_Vox_Crystal
SM_Chassis_Overcore_Crystal
SM_Chassis_Cryonix_Crystal
SM_Chassis_Clyffen_Crystal

Modelar:

front;
side;
3/4 silhouette

com leitura clara em tamanho pequeno.

Cada mesh deve possuir:

clean topology;
correct normals;
UV;
proper origin;
Unreal scale;
appropriate pivot.


# =====================================================================
# 39. FORGEKIN CRYSTAL — HIGH FIDELITY
# =====================================================================

FORGEKIN deve receber atenção especial porque é o chassis selecionado no target.

Forma:

volcanic crystal;
large central shard;
multiple side shards;
irregular;
sharp;
heavy;
mineral;
magma-like internal structure.

Material:

semi-transparent crystal;
orange-red;
deep black inclusions;
strong internal emissive;
bright edges;
hot core.

O cristal deve parecer:

forjado;
denso;
resistente;
vulcânico.


# =====================================================================
# 40. CRYSTAL MATERIAL SYSTEM
# =====================================================================

Criar material master compartilhável:

M_ChassisCrystal_Master

Parâmetros:

BaseColor
CoreColor
EmissiveColor
EmissiveIntensity
RefractionAmount
Opacity
Roughness
FresnelStrength
EdgeGlow
InternalNoiseScale
InternalNoiseSpeed
PulseSpeed
PulseStrength
SelectionGlow

Características:

transparency;
refraction;
controlled subsurface-like effect where appropriate;
fresnel;
internal emissive;
micro imperfections;
animated internal energy.

Evitar criar 11 shaders totalmente independentes sem necessidade.

Criar Material Instances por chassis.


# =====================================================================
# 41. CRYSTAL CORE
# =====================================================================

Criar núcleo emissivo interno.

O núcleo deve:

brilhar;
pulsar;
variar levemente;
iluminar partes internas do cristal.

FORGEKIN:

orange / magma.

AETHERIC:

emerald.

TECHNO:

electric blue.

etc.


# =====================================================================
# 42. CRYSTAL PLATFORM
# =====================================================================

Cada cristal flutua sobre uma plataforma tecnológica.

Criar asset compartilhado:

SM_ChassisCrystal_Platform

Características:

dark metal;
circular;
floating;
runes;
Ether ring;
small emissive sections;
energy projection.

A plataforma pode utilizar MI diferente por chassis.

Não criar onze plataformas geometricamente idênticas duplicadas se material
instances resolvem.


# =====================================================================
# 43. PLATFORM VFX
# =====================================================================

Criar:

NS_ChassisPlatform_Idle

Elementos:

runes;
orbital ring;
small floating particles;
holographic lines;
energy glow.

Selected crystal:

aumentar intensidade.

Não aumentar descontroladamente particle count.


# =====================================================================
# 44. CRYSTAL IDLE ANIMATION
# =====================================================================

Todos os cristais:

slow rotation;
slight levitation;
very subtle vertical oscillation;
small internal pulse.

Nunca parecer item completamente estático.

Movimento elegante.

Não utilizar Tick Blueprint para isso se:

Material Time;
Niagara;
Timeline;
animation system

puder resolver.


# =====================================================================
# 45. SELECTED CRYSTAL
# =====================================================================

No target:

FORGEKIN está selecionado.

Selected state:

strong frame;
orange/pink border;
slightly larger perceived size;
higher emissive;
stronger platform runes;
greater particles;
strong glow;
clear visual focus.

O estado deve ser reconhecível sem depender apenas do texto.


# =====================================================================
# 46. HOVER
# =====================================================================

Hover:

small scale/emphasis;
slight emissive increase;
small platform response;
highlight chassis name.

Transition:

aproximadamente 150–250ms.

Não usar animações exageradas.


# =====================================================================
# 47. CLYFFEN LOCKED STATE
# =====================================================================

Clyffen:

dimmed;
dark crystal;
amber accents;
lock icon.

Interaction:

hover pode mostrar informações.

Selection:

blocked.

Continue:

não deve aceitar Clyffen enquanto estado for Extinct/Locked.


# =====================================================================
# 48. NAVIGATION
# =====================================================================

Suportar:

Mouse
Keyboard
Gamepad

LEFT:
previous chassis.

RIGHT:
next chassis.

A / ENTER:
select.

B / ESC:
back.

Mouse:
hover/click.

Gamepad focus NÃO deve depender do mouse.


# =====================================================================
# 49. NÃO USE 11 SCENE CAPTURES
# =====================================================================

Evitar:

11 SceneCapture2D simultâneos.

Preferência:

criar os 11 cristais reais no menu world e visualizá-los através da camera
principal.

A UI pode possuir hit regions sobre os cristais ou sistema de interação
equivalente.

Isto preserva:

performance;
Lumen;
reflections;
visual coherence.


# =====================================================================
# 50. SELECTED FORGEKIN DETAIL PANEL
# =====================================================================

Na parte inferior esquerda:

FORGEKIN

CHASSI DE RESILIÊNCIA

Quote:

“Da pressão nasce a forma.”

Mostrar imagem/render do cristal Forgekin.

Exibir:

Tipo de Corpo
Stocky (Troncoso)

Afinidade de Éter
Estabilidade

Dificuldade
Média

Lore:

Forjados nas profundezas de Nexus-7, os Forgekin
são a muralha entre o colapso e a esperança.
Sua estrutura é projetada para resistir, proteger
e permanecer.


# =====================================================================
# 51. DETAIL PANEL DATA-DRIVEN
# =====================================================================

O painel NÃO é WBP_ForgekinPanel.

Criar:

WBP_ChassisDetails

Recebe:

FChassisDefinition

e renderiza qualquer chassis.

Forgekin aparece porque:

SelectedChassisId == Forgekin.


# =====================================================================
# 52. PLOIDREX SYSTEM
# =====================================================================

Título:

ATRIBUTOS INICIAIS (SISTEMA PLOIDREX)

Texto:

Distribuição fixa do chassi
(será ajustada por equipamentos e progressão).

IMPORTANTE:

PLOIDREX NÃO É D&D.

Não substituir automaticamente por:

Strength
Dexterity
Constitution
etc.

Usar os atributos canônicos:

VIGOR
ENGENHO
SINTONIA
REFLEXO
PRESENÇA
ADAPTAÇÃO


# =====================================================================
# 53. FORGEKIN ATTRIBUTES
# =====================================================================

VIGOR
12

Descrição:

Resistência física e vitalidade.


ENGENHO
8

Descrição:

Capacidade técnica e análise.


SINTONIA
6

Descrição:

Conexão e manipulação de Éter.


REFLEXO
8

Descrição:

Agilidade e tempo de reação.


PRESENÇA
6

Descrição:

Influência e interação.


ADAPTAÇÃO
10

Descrição:

Resiliência a ambientes e anomalias.


# =====================================================================
# 54. ATTRIBUTE COLOR LANGUAGE
# =====================================================================

Criar cores/ícones consistentes por atributo.

Exemplo conforme referência:

VIGOR:
red.

ENGENHO:
cyan / blue.

SINTONIA:
green.

REFLEXO:
violet.

PRESENÇA:
purple/pink.

ADAPTAÇÃO:
orange/yellow.

Essas cores devem vir do Design System/Data Asset.

Não hardcode dentro de cada row.


# =====================================================================
# 55. ATTRIBUTE COMPONENT
# =====================================================================

Criar:

WBP_AttributeRow

Recebe:

AttributeId
Icon
DisplayName
Value
Description
Color

Não duplicar seis estruturas independentes.


# =====================================================================
# 56. FORGEKIN ABILITIES
# =====================================================================

Título:

HABILIDADES DO CHASSI


PRIMÁRIA (2 UV)

ARMADURA FORJADA

Descrição:

Reduz 3 de dano físico por acerto.
+5 de HP no N1 (+1 por nível).


SECUNDÁRIA (1 UV)

BLINDAGEM ADAPTATIVA

Descrição:

+2 de CA no N1,
chegando a +5 no N20.


# =====================================================================
# 57. ABILITY COMPONENT
# =====================================================================

Criar:

WBP_ChassisAbilityCard

Recebe:

AbilityId
Name
Type
CostUV
Icon
Description
Color

Não codificar Forgekin diretamente.


# =====================================================================
# 58. GAMEPLAY STYLE
# =====================================================================

FORGEKIN:

ESTILO DE JOGO

TANQUE
SOBREVIVÊNCIA
CONTROLE DE ÁREA

Representar como tags/chips discretos.

Essas tags devem vir dos dados do chassis.


# =====================================================================
# 59. NEXT STEP PANEL
# =====================================================================

Painel inferior direito:

PRÓXIMO PASSO

Texto:

Após escolher sua essência, você
poderá personalizar o corpo, cores,
detalhes e aparência do seu Runner.

Primary button:

CONTINUAR

Secondary button:

VER CORPO DE EXEMPLO


# =====================================================================
# 60. CONTINUAR
# =====================================================================

CONTINUAR só pode executar se:

SelectedChassisId válido
AND
Chassis available
AND
CharacterCreationDraft válido.

Fluxo:

UI Continue Intent
↓
ContinueToAppearance Use Case
↓
Validate SelectedChassis
↓
Update Draft
↓
Change Step to Appearance
↓
Navigate to Step 2


# =====================================================================
# 61. STEP 2 CONTRACT
# =====================================================================

Step 2 deve receber o MESMO draft.

NÃO reconstruir dados utilizando estado visual.

SelectedChassisId deve permanecer.

Se o usuário voltar de Appearance para Essence:

restaurar chassis selecionado.


# =====================================================================
# 62. VER CORPO DE EXEMPLO
# =====================================================================

Este botão NÃO escolhe aparência definitiva.

A função serve para:

mostrar como aquele chassis pode se manifestar em um corpo representativo.

Pode abrir:

temporary preview;
3D preview;
modal;
camera transition;

seguindo arquitetura existente.

NÃO gravar AppearanceData final.

Implementar via:

PreviewExampleBody(SelectedChassisId)


# =====================================================================
# 63. WIZARD STATE
# =====================================================================

Possíveis estados:

NotStarted
LoadingCatalog
EssenceSelection
AppearanceSelection
Confirmation
Submitting
Completed
Error

Esses estados não devem ficar dispersos em booleans como:

bIsLoading
bIsStepOne
bIsConfirming
bIsFinished
bIsSomethingElse

Preferir estado explícito.


# =====================================================================
# 64. UMG ARCHITECTURE
# =====================================================================

Criar aproximadamente:

WBP_CharacterCreation_Chassis

Hierarchy conceitual:

CanvasPanel
 ├── WorldBackground
 ├── AtmosphericOverlay
 ├── Header
 │    ├── Logo
 │    ├── Title
 │    ├── CreationStepper
 │    └── TopQuote
 │
 ├── IntroPanel
 │    ├── Title
 │    ├── Description
 │    └── Quote
 │
 ├── ChassisCarousel
 │    ├── ChassisItem [...]
 │    └── Navigation
 │
 └── BottomArea
      ├── ChassisDetails
      ├── AttributesPanel
      ├── AbilitiesPanel
      └── NextStepPanel

Reusable:

WBP_CharacterCreationStepper
WBP_ChassisSelector
WBP_ChassisSelectorItem
WBP_ChassisDetails
WBP_AttributeRow
WBP_ChassisAbilityCard
WBP_GameplayStyleTag
WBP_PrimaryActionButton
WBP_SecondaryActionButton


# =====================================================================
# 65. PRESENTATION / VIEW MODEL
# =====================================================================

Se o projeto já utilizar Unreal MVVM:

criar ViewModel equivalente:

VM_CharacterCreationChassis

Expor:

ChassisItems
SelectedChassis
CreationStep
CanContinue
IsLoading
ErrorMessage

Caso MVVM não esteja adotado:

não introduzir tecnologia apenas por preferência.

Utilizar Presenter/ViewModel/service limpo equivalente.

A regra principal permanece:

Widget ≠ Domain Logic.


# =====================================================================
# 66. LOCALIZATION
# =====================================================================

Todos os textos de UI devem utilizar:

FText.

Preferir:

String Tables / Localization system.

Não usar FString hardcoded para conteúdo apresentado ao jogador.

Preparar estrutura para:

PT-BR
EN
FR

e suporte futuro.

Texto japonês do logo deve reutilizar asset aprovado do logo.


# =====================================================================
# 67. MATERIALS — UI
# =====================================================================

Reutilizar quando possível os materiais criados anteriormente:

M_UI_CyberFrame
M_UI_EnergyBorder
M_UI_EnergySweep
M_UI_Glass
M_UI_Scanline
M_UI_HolographicNoise

Se já existirem:

NÃO recriá-los.

Criar novas Material Instances apenas quando necessário.


# =====================================================================
# 68. MATERIALS — CRYSTAL
# =====================================================================

Criar:

M_ChassisCrystal_Master

Material Instances:

MI_Chassis_Charger
MI_Chassis_Vitaspark
MI_Chassis_Aetheric
MI_Chassis_Techno
MI_Chassis_Droneframe
MI_Chassis_Forgekin
MI_Chassis_Ghostnet
MI_Chassis_Vox
MI_Chassis_Overcore
MI_Chassis_Cryonix
MI_Chassis_Clyffen


# =====================================================================
# 69. MATERIAL DETAIL PASSES
# =====================================================================

A referência define visualmente:

CRYSTAL BASE

transparência;
refração;
subsurface-like scattering;
faceted structure.


EMISSIVE CORE

pulsação;
energy variation.


EDGE DETAILS

roughness;
micro scratches;
minor imperfections.


ETHER RING

additive;
fresnel;
noise;
animated orbit.


PLATFORM

dark metallic;
runes holográficas;
emissive.


PARTICLES

sprites/meshes;
noise;
scale variation;
spark variation.


# =====================================================================
# 70. VFX PASSES
# =====================================================================

Cada cristal pode possuir composição:

1. EMISSIVE CORE

2. ENERGY AURA

3. PARTICLES / FRAGMENTS

4. ORBITAL LINES

5. RUNES / HOLOGRAMS

6. SELECTION GLOW


# =====================================================================
# 71. FORGEKIN VFX
# =====================================================================

Forgekin deve possuir:

orange internal core;
lava-like pulses;
small ember fragments;
floating crystal debris;
orange orbital runes;
selection bloom.

Evitar aparência de fogo convencional.

É energia de Éter dentro de cristal vulcânico.


# =====================================================================
# 72. NIAGARA
# =====================================================================

Utilizar Niagara para:

crystal particles;
floating fragments;
runes;
energy rings;
background nanobots;
distant drones;
fog accents;
lightning;
small sparks.

Não usar um Niagara System gigante para tudo.

Separar responsabilidades.


# =====================================================================
# 73. BACKGROUND VFX REUSE
# =====================================================================

Reutilizar sistemas já criados para telas anteriores:

nano swarm;
storm lightning;
volumetric atmosphere;
floating structures;

se já existirem.

Não criar:

NS_Login_Nano
NS_Selection_Nano
NS_Creation_Nano

com três implementações quase idênticas.

Criar sistema compartilhado configurável.


# =====================================================================
# 74. LUMEN / LIGHTING
# =====================================================================

Utilizar Lumen quando adequado ao projeto.

Cristais devem iluminar discretamente:

platform;
fog;
nearby geometry.

Forgekin:

orange contribution.

Vox:

magenta.

Aetheric:

green.

etc.

Controlar exposure.

Não deixar emissive estourar toda geometria para branco.


# =====================================================================
# 75. POST PROCESS
# =====================================================================

Utilizar com moderação:

Bloom
Chromatic Aberration
Vignette
Color Grading
Fog
Lens effects

Prioridade:

legibilidade.

Chromatic aberration deve ser quase imperceptível.

Não destruir o texto.


# =====================================================================
# 76. CAMERA
# =====================================================================

Composição semelhante ao target.

Background:

Nexus-7.

Middle layer:

crystals.

Foreground:

UI.

Adicionar parallax sutil ao mundo.

UI screen-space NÃO deve acompanhar parallax.

Mouse/gamepad:

máximo aproximadamente 1–3 graus perceptivos.


# =====================================================================
# 77. AUDIO HOOKS
# =====================================================================

Preparar:

SFX_UI_Hover
SFX_UI_Select
SFX_UI_Back
SFX_UI_Continue
SFX_Chassis_Selected
SFX_Chassis_Locked
SFX_Chassis_Preview

Ambient:

Nexus7_Atmosphere

Crystals:

very subtle hum.

Forgekin:

lower-frequency energy character.

Não exigir áudio final se assets ainda não existirem.

Preparar hooks.


# =====================================================================
# 78. COMFYUI ROLE
# =====================================================================

ComfyUI é ferramenta auxiliar de produção.

Pode gerar:

crystal concepts;
shape references;
surface references;
Nexus-7 mood;
texture masks;
roughness ideas;
emissive masks;
normal/depth support;
environment concepts;
VFX reference;
alternative designs.

NÃO usar ComfyUI para gerar a UI inteira como uma imagem e colocar no Unreal.

NÃO substituir os cristais 3D finais por concept images quando meshes são
necessários.


# =====================================================================
# 79. BLENDER ROLE
# =====================================================================

Blender MCP deve produzir assets 3D faltantes:

11 crystal models;
platform;
optional environment props;
distant floating structures quando ainda não existirem.

Aplicar:

hard-surface / crystal modeling;
clean transforms;
correct scale;
proper pivot;
normals;
UV;
LOD strategy where appropriate.

Não aplicar dezenas de milhões de polígonos apenas porque Nanite existe.


# =====================================================================
# 80. ASSET NAMING
# =====================================================================

Primeiro descubra a convenção existente no projeto.

SE EXISTIR:

SIGA A CONVENÇÃO EXISTENTE.

Se ainda não houver padrão definido:

use nomenclatura Unreal clara:

PDA_Chassis_Forgekin
SM_Chassis_Forgekin_Crystal
MI_Chassis_Forgekin
NS_Chassis_Forgekin_Idle
NS_Chassis_Forgekin_Selected
T_Chassis_Forgekin_Mask
WBP_ChassisSelectorItem

Não misture múltiplas convenções.


# =====================================================================
# 81. PROJECT ORGANIZATION
# =====================================================================

Sugestão sem duplicar estruturas existentes:

/Game/AetherForge/UI/Common/

/Game/AetherForge/UI/CharacterCreation/

/Widgets
/ViewModels
/Animations
/Icons

/Game/AetherForge/Characters/Creation/

/DomainData
/Chassis
/Preview

/Game/AetherForge/Environment/Nexus7/

/Game/AetherForge/VFX/

/Game/AetherForge/Materials/


# =====================================================================
# 82. RESPONSIVE DESIGN
# =====================================================================

Target canônico:

1536 × 1024.

Testar também:

1920×1080
2560×1440
3440×1440
3840×2160

No target:

seguir fortemente a composição original.

Em 16:9:

expandir principalmente background.

Não deformar painel.

Em ultrawide:

mais Nexus-7 nas laterais.

Não espalhar UI pelo monitor inteiro.

Usar:

Anchors
SafeZone
ScaleBox somente quando adequado
HorizontalBox
VerticalBox
Overlay
SizeBox
dynamic carousel.

Evitar absolute layout para tudo.


# =====================================================================
# 83. TARGET HEADER
# =====================================================================

SUPERIOR ESQUERDO:

logo AETHER FORGE X
PROTOCOLO ZERO

CENTRO:

CRIAÇÃO DE PERSONAGEM

Stepper diretamente abaixo.

SUPERIOR DIREITO:

UM NÚCLEO. INFINITAS POSSIBILIDADES.

“ESCOLHA O QUE TE MOVE.”


# =====================================================================
# 84. TARGET CENTER
# =====================================================================

A cidade Nexus-7 deve estar claramente visível atrás.

Texto ambiental:

NEXUS-7

ALÉM DA NÉVOA,
UM NOVO AMANHÃ.

A grande cidade deve surgir da neblina violeta.


# =====================================================================
# 85. TARGET CRYSTAL ROW
# =====================================================================

Na mesma leitura horizontal do target:

CHARGER
VITASPARK
AETHERIC
TECHNO
DRONEFRAME
FORGEKIN
GHOSTNET
VOX
OVERCORE
CRYONIX
CLYFFEN

Forgekin aproximadamente no centro.

Cada chassis mostra:

icon;
crystal;
name;
identity;
small description.

Forgekin possui selection frame forte.


# =====================================================================
# 86. BOTTOM LAYOUT
# =====================================================================

Parte inferior dividida aproximadamente em:

[ CHASSIS DETAILS ]

[ INITIAL ATTRIBUTES ]

[ CHASSIS ABILITIES ]

[ NEXT STEP ]

Sem gaps enormes.

Manter densidade sci-fi do target.


# =====================================================================
# 87. FOOTER
# =====================================================================

Inferior esquerdo:

AETHER FORGE X | PROTOCOLO ZERO

Inferior direito:

IDENTIDADE. EVOLUÇÃO. PROPÓSITO.

Visual:

subtle;
small;
high tracking;
muted cyan/gray.


# =====================================================================
# 88. CURRENT SELECTION STATE
# =====================================================================

No VISUAL TEST que deverá reproduzir a reference:

SelectedChassisId = Forgekin

Portanto screenshot esperado deve mostrar:

FORGEKIN selected;
orange crystal;
orange selected frame;
Forgekin details;
Forgekin attributes;
Forgekin abilities.

Isso é TEST FIXTURE / ART VALIDATION.

NÃO transformar Forgekin em seleção fixa de runtime.


# =====================================================================
# 89. DRAFT RESTORATION
# =====================================================================

Ao retornar do Step 2:

se Draft.SelectedChassisId existir:

restaurar seleção.

Não voltar arbitrariamente para primeiro item.


# =====================================================================
# 90. EMPTY DRAFT
# =====================================================================

Quando um novo Draft é criado e ainda não existe SelectedChassisId:

usar comportamento definido pelo UX.

Preferência segura:

nenhum chassis confirmado até interação explícita

OU

primeiro chassis available como focus visual sem persistir seleção.

NÃO gravar seleção apenas porque recebeu focus.


# =====================================================================
# 91. SELECT VS FOCUS
# =====================================================================

Distinguir:

FOCUSED
HOVERED
SELECTED

FOCUS:

navegação.

HOVER:

mouse.

SELECTED:

escolha atual do draft.

Não trate focus como confirmação.


# =====================================================================
# 92. CHASSIS AVAILABILITY
# =====================================================================

A biblioteca de chassis é conteúdo do jogo.

Disponibilidade pode depender de:

Game Rules
Progression
Account Entitlement
Unlock
Story
Live Content

A UI recebe:

Available
Locked
Extinct

Não decide.

Atualmente:

Clyffen = Extinct/Locked.

Os demais aparecem como jogáveis conforme referência.


# =====================================================================
# 93. DOMAIN INVARIANTS
# =====================================================================

SelectChassis deve recusar:

invalid ChassisId;
unknown chassis;
locked chassis;
extinct chassis;
unavailable chassis.

ContinueToAppearance deve exigir:

valid SelectedChassisId.

CreateCharacter final deve exigir:

valid account;
free slot;
valid chassis;
valid appearance;
valid confirmation data.

Mesmo que a UI esteja bugada:

Domain/Application validation deve continuar protegendo regras.


# =====================================================================
# 94. ROUTING / NAVIGATION
# =====================================================================

Não usar OpenLevel diretamente dentro de qualquer botão se já existir sistema
central de navegação.

Preferir interface equivalente:

IMenuNavigationService

Rotas:

Login
CharacterSelection
CharacterCreation.Essence
CharacterCreation.Appearance
CharacterCreation.Confirmation

Screen → request navigation.

Navigator → decide implementação.


# =====================================================================
# 95. INTEGRATION WITH CHARACTER SELECTION
# =====================================================================

Na tela anterior:

CRIAR NOVO

deve chamar:

StartCharacterCreation

e somente navegar se:

CanCreateCharacter == true.

Ao finalizar criação:

CharacterCreated event

deve provocar:

RefreshCharacterRoster

e selecionar preferencialmente:

new CharacterId.


# =====================================================================
# 96. CANCEL FLOW
# =====================================================================

Se usuário pressionar BACK na criação:

se Draft ainda não possuir alterações significativas:

retornar à Character Selection.

Se houver alterações:

pode utilizar confirmation:

DESCARTAR CRIAÇÃO?

Seguindo padrão UX do projeto.

Cancelar NÃO cria personagem.


# =====================================================================
# 97. SAVE / BACKEND
# =====================================================================

Domain e UI não devem depender de implementação concreta.

Infraestrutura pode ser:

REST API;
local SaveGame;
database adapter;
mock service;

sem reescrever UMG.

Use dependency inversion.


# =====================================================================
# 98. ERROR HANDLING
# =====================================================================

Estados possíveis:

CatalogLoadFailed
AccountUnavailable
NoCharacterSlot
ChassisUnavailable
DraftSaveFailed
NavigationFailed

Não usar crash ou Silent Fail.

Apresentar erro de forma consistente com Login e Character Selection.


# =====================================================================
# 99. ASYNC
# =====================================================================

Operações backend podem ser assíncronas.

Nunca congelar UI.

Exibir loading state quando necessário.

Evitar double-submit.

Enquanto request está em andamento:

desabilitar ação correspondente.


# =====================================================================
# 100. TESTABILITY
# =====================================================================

A lógica central precisa ser testável sem abrir toda a tela.

Criar testes para:

StartCharacterCreation

SelectAvailableChassis

RejectLockedChassis

ContinueWithoutSelectionFails

ContinueWithValidSelectionSucceeds

DraftPreservesSelectedChassis

CancelDoesNotCreateCharacter

CharacterNotAddedBeforeConfirmation

FullAccountCannotStartCreation


# =====================================================================
# 101. CHASSIS CATALOG TESTS
# =====================================================================

Validar:

11 definitions registered.

IDs unique.

SortOrder unique.

Every chassis has:

name;
color;
mesh reference;
icon;
availability;
identity.

Clyffen:

Extinct/Locked.

Forgekin test data:

correct attributes;
correct ability data.


# =====================================================================
# 102. VISUAL TESTING
# =====================================================================

Criar forma reprodutível de visualizar:

Forgekin selected.

Executar em:

1536×1024.

Capturar screenshot.

Comparar com:

TCPC_Tela_Criacao_Personagem_Chassi_art_final.png

Não declarar concluído simplesmente porque compila.


# =====================================================================
# 103. VISUAL COMPARISON LOOP
# =====================================================================

IMPLEMENT
↓
PLAY
↓
CAPTURE SCREENSHOT
↓
COMPARE WITH TARGET
↓
IDENTIFY DIFFERENCES
↓
FIX
↓
PLAY AGAIN
↓
CAPTURE AGAIN

Repetir.


# =====================================================================
# 104. COMPARAR
# =====================================================================

Verificar:

logo position;
title position;
stepper position;
top-right quote;
intro position;
crystal row;
crystal sizes;
chassis labels;
Forgekin highlight;
Nexus-7 position;
fog density;
purple storm;
orange Forgekin intensity;
bottom panel height;
column widths;
attribute rows;
ability cards;
next-step panel;
button sizes;
footer;
contrast;
typography;
spacing;
bloom.


# =====================================================================
# 105. PERFORMANCE
# =====================================================================

Evitar:

Blueprint Tick desnecessário;
11 SceneCapture components;
gigantic 8K UI textures;
duplicated materials;
duplicated crystal shaders;
unbounded Niagara particles;
constant dynamic allocations;
GetAllActorsOfClass loops.

Usar:

shared Master Materials;
Material Instances;
Niagara LOD;
soft references;
Asset Manager;
timelines;
material animation;
event-driven UI.


# =====================================================================
# 106. ACCESSIBILITY / USABILITY
# =====================================================================

Apesar da estética neon:

texto deve permanecer legível.

Não depender apenas de cor para estado.

Selected deve possuir:

frame;
glow;
position/emphasis.

Locked deve possuir:

lock icon;
reduced saturation.

Gamepad focus deve possuir indicação clara.


# =====================================================================
# 107. NO MAGIC NUMBERS
# =====================================================================

Não espalhar:

colors;
animation durations;
glow intensities;
spacing;
font sizes;

por dezenas de Blueprints.

Criar:

UI Theme/Data Asset
ou configuração central equivalente.


# =====================================================================
# 108. LOGGING
# =====================================================================

Criar logging útil por categoria.

Exemplos conceituais:

LogCharacterCreation
LogChassisCatalog
LogMenuNavigation

Não encher Output Log a cada frame.


# =====================================================================
# 109. NAMING / CLEAN CODE
# =====================================================================

Métodos devem expressar intenção.

BOM:

SelectChassis
CanContinueToAppearance
LoadChassisCatalog
RefreshSelectedChassisView
CreateCharacterCreationDraft

RUIM:

DoThing
HandleStuff
Button1
CheckData2
TempFunction


# =====================================================================
# 110. DOCUMENTATION
# =====================================================================

Documentar especialmente:

CharacterCreation flow;
Aggregate invariants;
Chassis catalog;
Data assets;
navigation;
how to add a new chassis.

Adicionar novo chassis futuramente deve exigir principalmente:

1. new data asset;
2. mesh/material/VFX;
3. game rules data.

NÃO alterações em 15 widgets e switch statements.


# =====================================================================
# 111. ADDING FUTURE CHASSIS
# =====================================================================

O sistema deve suportar:

12th chassis;
13th chassis;
event chassis;
unlockable chassis;

sem reescrever a tela.

O carousel deve ser gerado a partir do catalog.


# =====================================================================
# 112. SOURCE OF TRUTH
# =====================================================================

GAME RULE DATA:

define o que cada chassis É.

PLAYER ACCOUNT / PROGRESSION:

define o que está DISPONÍVEL para aquele jogador.

CHARACTER CREATION DRAFT:

define o que o jogador SELECIONOU nessa criação.

UMG:

define COMO isso é mostrado.

Nunca misturar essas responsabilidades.


# =====================================================================
# 113. FORGEKIN REFERENCE DATA — FULL
# =====================================================================

FORGEKIN

Identity:

DEFESA

Archetype:

CHASSI DE RESILIÊNCIA

Quote:

“Da pressão nasce a forma.”

Crystal:

Cristal Vulcânico

Body Type Hint:

Stocky (Troncoso)

Ether Affinity:

Estabilidade

Difficulty:

Média

Lore:

Forjados nas profundezas de Nexus-7, os Forgekin
são a muralha entre o colapso e a esperança.
Sua estrutura é projetada para resistir, proteger
e permanecer.

Base Attributes:

VIGOR = 12
ENGENHO = 8
SINTONIA = 6
REFLEXO = 8
PRESENÇA = 6
ADAPTAÇÃO = 10

Primary Ability:

ARMADURA FORJADA

Cost:

2 UV

Effect:

Reduz 3 de dano físico por acerto.
+5 de HP no N1 (+1 por nível).

Secondary Ability:

BLINDAGEM ADAPTATIVA

Cost:

1 UV

Effect:

+2 de CA no N1,
chegando a +5 no N20.

Style Tags:

TANQUE
SOBREVIVÊNCIA
CONTROLE DE ÁREA


# =====================================================================
# 114. REQUIRED SCREEN TEXT
# =====================================================================

CRIAÇÃO DE PERSONAGEM

1 ESSÊNCIA (CHASSI)
2 APARÊNCIA (CORPO)
3 CONFIRMAÇÃO

ESCOLHA SUA ESSÊNCIA

Cada cristal de Éter é um Chassi.
Ele define sua afinidade, atributos iniciais,
habilidades e sua relação com o Éter.
O corpo vem depois.

“O cristal escolhe quem pode carregá-lo,
mas o que você se torna, é com você.”

NEXUS-7

ALÉM DA NÉVOA,
UM NOVO AMANHÃ.

UM NÚCLEO. INFINITAS POSSIBILIDADES.

“ESCOLHA O QUE TE MOVE.”

CHARGER
IMPACTO

VITASPARK
VELOCIDADE

AETHERIC
HARMONIA

TECHNO
TECNOLOGIA

DRONEFRAME
SUPORTE

FORGEKIN
DEFESA

GHOSTNET
INFILTRAÇÃO

VOX
PRESENÇA

OVERCORE
PODER

CRYONIX
CONTROLE

CLYFFEN
LENDÁRIO

ATRIBUTOS INICIAIS (SISTEMA PLOIDREX)

HABILIDADES DO CHASSI

ESTILO DE JOGO

PRÓXIMO PASSO

CONTINUAR

VER CORPO DE EXEMPLO

AETHER FORGE X | PROTOCOLO ZERO

IDENTIDADE. EVOLUÇÃO. PROPÓSITO.


# =====================================================================
# 115. ART DIRECTION PHRASES
# =====================================================================

Estas frases servem principalmente de direção narrativa:

“O CRISTAL ESCOLHE QUEM PODE CARREGÁ-LO,
MAS O QUE VOCÊ SE TORNA, É COM VOCÊ.”

“PRIMEIRO, A ESSÊNCIA.
DEPOIS, A FORMA.
SEMPRE O PROPÓSITO.”

“UM NÚCLEO. INFINITAS POSSIBILIDADES.”

“NEXUS-7 — MAIS QUE RUÍNAS. É O RECOMEÇO.”

Não colocar todas aleatoriamente dentro do HUD.

Utilizar somente onde definido pela composição.


# =====================================================================
# 116. DO NOT DO
# =====================================================================

NÃO:

reconstruir Login;
reconstruir Character Selection;
duplicar Design System;
duplicar Nexus-7;
duplicar logo;
duplicar fonts;
duplicar materials sem necessidade;

hardcode Forgekin na tela;
hardcode 11 widgets diferentes;
hardcode availability no UMG;
hardcode backend no Blueprint de UI;

criar personagem antes da Confirmation;
adicionar Draft ao Character Roster;

tratar Focus como Selection;

usar Clyffen como disponível;

inventar stats dos outros chassis;

usar D&D no lugar do Ploidrex;

criar 11 SceneCapture2D;

usar Tick para cada cristal;

criar tela estática como PNG;

usar apenas ComfyUI para simular implementação;

criar God Widget;

criar God Blueprint;

misturar Domain Logic com Presentation.


# =====================================================================
# 117. PIPELINE DE IMPLEMENTAÇÃO OBRIGATÓRIO
# =====================================================================

FASE 1 — DISCOVERY

Inspecione:

current project;
Login implementation;
Character Selection implementation;
common UI;
navigation;
GameInstance;
account/session;
SaveGame;
backend adapters;
input;
existing Nexus-7 assets;
existing materials;
existing Niagara systems.


FASE 2 — ARCHITECTURE

Identifique:

o que reutilizar;
o que refatorar para Common;
o que realmente precisa ser criado.


FASE 3 — DOMAIN

Implementar/refinar:

CharacterCreationDraft;
ChassisDefinition;
PloidrexAttributes;
creation state;
domain/application validation.


FASE 4 — DATA

Criar:

Chassis catalog;
Forgekin canonical data;
11 chassis entries;
availability.


FASE 5 — BLENDER

Criar crystals/platform assets faltantes.


FASE 6 — MATERIALS

Criar master materials e instances.


FASE 7 — VFX

Criar crystal VFX;
platform VFX;
reuse Nexus VFX.


FASE 8 — WORLD COMPOSITION

Criar camera/composition que reproduza o target.


FASE 9 — UI

Criar UMG reusable components.


FASE 10 — INTERACTION

Mouse;
keyboard;
gamepad;
hover;
focus;
selected;
locked.


FASE 11 — APPLICATION INTEGRATION

Start;
select;
continue;
back;
preview.


FASE 12 — PREVIOUS SCREEN INTEGRATION

Character Selection
→ Create New
→ Character Creation


FASE 13 — NEXT STEP CONTRACT

Essence
→ Appearance


FASE 14 — TESTS

Domain;
application;
catalog;
navigation.


FASE 15 — VISUAL VALIDATION

Play;
screenshot;
compare;
fix.


# =====================================================================
# 118. ACCEPTANCE CRITERIA — ARCHITECTURE
# =====================================================================

A tarefa NÃO está concluída se:

CharacterCreation depende diretamente de WBP_LoginScreen.

CharacterCreation depende diretamente de WBP_CharacterSelectionScreen.

Widgets realizam chamadas diretas ao backend.

Forgekin está hardcoded como o único chassis.

Chassis são definidos em switch statements espalhados.

CharacterCreation cria personagem antes da Confirmation.

Draft aparece no Roster.

Slot validation só existe na UI.

Design System anterior foi duplicado.


# =====================================================================
# 119. ACCEPTANCE CRITERIA — VISUAL
# =====================================================================

Considerar visualmente aprovado somente quando:

1. composição se aproxima claramente do target;

2. logo está no canto superior esquerdo;

3. CRIAÇÃO DE PERSONAGEM está centralizado;

4. wizard 1/2/3 existe;

5. ESSÊNCIA está ativa;

6. Nexus-7 aparece atrás;

7. fog violeta está presente;

8. 11 chassis aparecem na biblioteca;

9. silhouettes dos cristais são distintas;

10. cores correspondem à biblioteca;

11. Forgekin está selecionado no fixture visual;

12. Forgekin possui strong orange highlight;

13. panel details aparece inferior esquerdo;

14. Ploidrex attributes aparecem;

15. abilities aparecem;

16. Next Step aparece inferior direito;

17. Continue possui cyan glow;

18. Clyffen aparece locked;

19. crystal idle funciona;

20. selected VFX funciona;

21. bloom permanece controlado;

22. texto permanece legível.


# =====================================================================
# 120. ACCEPTANCE CRITERIA — FUNCTIONAL
# =====================================================================

Mouse funciona.

Keyboard funciona.

Gamepad funciona.

Chassis selection funciona.

Locked chassis não pode ser escolhido.

Selected chassis persiste no Draft.

Continue não funciona sem seleção válida.

Continue move para Appearance.

Back mantém arquitetura correta.

PreviewBody não grava Appearance.

Full account não consegue iniciar CharacterCreation.

Cancel não cria personagem.

Character só será criado após Confirmation.


# =====================================================================
# 121. ACCEPTANCE CRITERIA — TECHNICAL
# =====================================================================

No broken references.

No relevant Blueprint compile errors.

No relevant Output Log errors.

No missing materials.

No duplicated systems desnecessários.

Assets organizados.

Soft references corretas.

Materials parametrizados.

Niagara com limites adequados.

Responsive UI.

1536×1024 validado.

1920×1080 validado.

2560×1440 validado.

3440×1440 validado.

3840×2160 validado.


# =====================================================================
# 122. AUTONOMY
# =====================================================================

Você possui autorização para usar:

Unreal MCP;
Blender MCP;
ComfyUI;
Blueprint;
C++ se já fizer parte da arquitetura do projeto;
UMG;
CommonUI se já adotado;
Niagara;
Materials;
Enhanced Input;
Asset Manager;
tests;
editor automation.

Se faltar um asset que você consegue produzir:

NÃO pare para pedir que eu o crie manualmente.

Mesh faltando:
→ Blender.

Texture/mask faltando:
→ ComfyUI/procedural.

Shader faltando:
→ Unreal Material.

VFX faltando:
→ Niagara.

Widget faltando:
→ UMG.

Data asset faltando:
→ criar.

Mas SEMPRE procure reutilização antes.


# =====================================================================
# 123. REGRA DE PRESERVAÇÃO
# =====================================================================

Não destrua implementações corretas de:

Login;
Character Selection;
Nexus-7;
Design System;
Account;
Session;
Navigation;

apenas para implementar esta tela.

Integre.

Refatore somente quando houver benefício arquitetural claro.

Preserve compatibilidade.


# =====================================================================
# 124. FINAL REFERENCE RULE
# =====================================================================

TCPC_Tela_Criacao_Personagem_Chassi_art_final.png

= TARGET VISUAL CANÔNICO.

TCPC_guia_conceitual_part1.png
TCPC_guia_conceitual_part2.png
TCPC_guia_conceitual_part3.png

= DOCUMENTAÇÃO DE IMPLEMENTAÇÃO.

Os prompts e sistemas anteriores de:

LOGIN
CHARACTER SELECTION

= CONTEXTO ARQUITETURAL EXISTENTE.

Não trate Character Creation como projeto separado.


# =====================================================================
# 125. FINAL EXECUTION INSTRUCTION
# =====================================================================

COMECE AGORA.

1. Analise cuidadosamente as quatro imagens.

2. Inspecione a implementação atual do projeto Unreal Engine 5.8.

3. Localize o que já existe de Login, Account, Session, Character Selection,
   Navigation, Design System, Nexus-7, Materials e Niagara.

4. Não duplique assets ou serviços já existentes.

5. Identifique corretamente os bounded contexts.

6. Crie/refine CharacterCreation como domínio independente da Presentation.

7. Implemente CharacterCreationDraft.

8. Implemente o Chassis Catalog data-driven.

9. Configure os 10 chassis jogáveis + Clyffen extinto.

10. Modele no Blender somente os assets realmente faltantes.

11. Use ComfyUI somente como suporte artístico.

12. Monte a composição Nexus-7 reutilizando o ambiente existente.

13. Crie os cristais e seus VFX.

14. Crie a interface com componentes reutilizáveis.

15. Implemente seleção, hover, focus, locked e selected.

16. Implemente Ploidrex attributes.

17. Implemente chassis abilities.

18. Implemente o Next Step.

19. Integre mouse, keyboard e gamepad.

20. Integre com o Draft e Application Use Cases.

21. Integre com Character Selection.

22. Prepare a navegação para Appearance.

23. NÃO crie o personagem definitivo nesta etapa.

24. Execute a tela.

25. Use Forgekin selecionado no fixture visual para corresponder ao target.

26. Capture screenshot em 1536×1024.

27. Compare diretamente com:

TCPC_Tela_Criacao_Personagem_Chassi_art_final.png

28. Corrija:

layout;
spacing;
crystal scale;
camera;
fog;
lighting;
colors;
Forgekin;
panels;
typography;
VFX;
bloom.

29. Execute novamente.

30. Continue o loop até atingir alta correspondência visual e funcional.

NÃO termine a tarefa apenas apresentando um plano.

NÃO termine quando os widgets simplesmente compilarem.

NÃO declare sucesso sem executar e validar.

A tarefa termina somente quando esta etapa do Character Creation estiver
funcionando dentro do Unreal Engine 5.8, integrada ao fluxo já existente,
arquiteturalmente limpa, data-driven, testável, responsiva e visualmente próxima
de TCPC_Tela_Criacao_Personagem_Chassi_art_final.png.
