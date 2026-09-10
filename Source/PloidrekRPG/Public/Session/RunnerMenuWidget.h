#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/RunnerTypes.h"
#include "RunnerMenuWidget.generated.h"

class UButton;
class UCanvasPanel;
class UEditableTextBox;
class UTexture2D;
class UScrollBox;
class UTextBlock;
class UVerticalBox;
class URunnerMenuWidget;

/** Passos do fluxo de entrada. */
UENUM()
enum class ERunnerMenuStep : uint8
{
    Login,
    CreateAccount,
    /** Depois do login: escolher um personagem da conta ou criar um novo. */
    CharacterSelect,
    Chassis,
    Class,
    /** Janela de confirmacao: criar o personagem ou apagar um que existe. */
    Confirm
};

/** O que a linha da lista faz quando clicada. */
UENUM()
enum class ERunnerOptionAction : uint8
{
    /** Escolhe o item (chassi/classe na grade). */
    Jogar,
    /** Marca um Runner da lista, sem entrar no jogo. */
    Selecionar,
    Excluir
};

/** O que a janela de confirmacao executa se o jogador confirmar. */
UENUM()
enum class ERunnerConfirmAction : uint8
{
    Nada,
    CriarPersonagem,
    ExcluirPersonagem
};

/** Ligacao entre uma aba (CHASSI/CLASSE) e o passo que ela abre. */
UCLASS()
class PLOIDREKRPG_API URunnerTabButton : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY() TObjectPtr<URunnerMenuWidget> Owner;
    UPROPERTY() ERunnerMenuStep Destino = ERunnerMenuStep::Chassis;

    UFUNCTION()
    void HandleClicked();
};

/** Ligacao entre um botao de opcao (sem parametros) e o id que ele representa. */
UCLASS()
class PLOIDREKRPG_API URunnerOptionButton : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY() FName OptionId;
    UPROPERTY() TObjectPtr<URunnerMenuWidget> Owner;
    UPROPERTY() ERunnerOptionAction Action = ERunnerOptionAction::Jogar;
    /** Guardado para o caso do personagem da lista (chave da conta + id). */
    UPROPERTY() FString CharacterId;

    UFUNCTION()
    void HandleClicked();

    /** Passar o mouse ja mostra aquele chassi/classe/personagem na vitrine. */
    UFUNCTION()
    void HandleHovered();
};

/**
 * Menu do jogo em C++/UMG: entrar -> criar conta -> escolher chassi -> escolher classe -> criar personagem.
 * A tela nao decide nada: toda regra fica em URunnerSessionSubsystem / URunnerRules.
 */
UCLASS()
class PLOIDREKRPG_API URunnerMenuWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeOnInitialized() override;
    virtual void NativeDestruct() override;

    /** Escolhe o chassi e avanca para a classe. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Menu")
    void SelectChassis(FName ChassisId);

    /** Escolhe a classe. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Menu")
    void SelectClass(FName ClassId);

    /** Passo atual. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Menu")
    void GoToStep(ERunnerMenuStep NewStep);

    /** Atualiza a vitrine 3D e a janela de atributos com o que o jogador esta olhando. */
    void PrevisualizarOpcao(const FName& OptionId, ERunnerOptionAction Action, const FString& CharacterId);

    /** Abre a janela de confirmacao (criar personagem / apagar personagem). */
    void AbrirConfirmacao(ERunnerConfirmAction Acao, const FString& Mensagem, const FString& CharacterId);

    /** Troca de aba na tela de criacao (chassi <-> classe). */
    void TrocarAba(ERunnerMenuStep Aba);

    /** Marca um Runner da lista como o escolhido (mostra na vitrine e nos detalhes). */
    void SelecionarPersonagem(const FString& CharacterId, bool bEntrarNoJogo);

    UFUNCTION() void OnQuartoClicked();

protected:
    void RebuildLayout();
    void SetStatus(const FString& Message);

    /** Sessao do jogador: a maquina de estado da conta e da criacao. */
    class URunnerSession* GetSession() const;

    UFUNCTION() void OnPrimaryClicked();
    UFUNCTION() void OnSecondaryClicked();
    UFUNCTION() void OnTertiaryClicked();

    /** Resultado do login (o EOS responde assincrono; o local responde na hora). */
    UFUNCTION() void HandleLoginComplete(bool bSuccess, const FString& Error);

    /** Depois de entrar: lista os personagens da conta, ou vai direto criar o primeiro. */
    void AfterLogin();

    /** Troca para o GameMode do jogo (o pawn nasce e recebe o corpo). */
    void OpenGameLevel();

    // ---------- Vitrine 3D + janela de atributos ----------
    /** Cria (uma vez) ou reaproveita o ator da vitrine e devolve a imagem que o mostra. */
    UWidget* CriarBlocoDaVitrine();
    /** Coluna da direita: nome, corpo, atributos, vantagens e a conta de HP/CA. */
    UWidget* CriarBlocoDeInfo();
    /** Linha de atributo: nome a esquerda, valor e modificador a direita. */
    UWidget* CriarLinhaDeAtributo(ERunnerAttribute Atributo, int32 Bonus);
    /** Preenche a info com o chassi (e a classe, quando ja escolhida). */
    void PreencherInfo(UVerticalBox* Caixa, FName ChassisId, FName ClassId, const FString& TituloDoRunner = FString());
    /** A vitrine mostra o corpo deste chassi. */
    void MostrarNaVitrine(FName ChassiId);
    /** Termina o ator da vitrine (ao sair do menu). */
    void DestruirVitrine();

    // ---------- Telas ----------
    /**
     * Tela de criacao (abas chassi/classe): ficha a esquerda, vitrine no meio, detalhes a direita,
     * abas, grade de icones e o nome do Runner embaixo.
     */
    void ConstruirTelaDeCriacao(TArray<UWidget*>& AlvosDoVeu);

    /** Tela dos Runners que ja existem: lista a esquerda, vitrine no meio, detalhes a direita. */
    void ConstruirTelaDeRunners(TArray<UWidget*>& AlvosDoVeu);

    /** Coluna da esquerda: a ficha resumida (chassi, classe, atributos, HP/CA). */
    UWidget* CriarColunaDaFicha();

    /** Coluna da direita: o detalhe do que esta selecionado, com o icone. */
    UWidget* CriarColunaDeDetalhes();

    /** Quantas colunas a grade de icones tem. */
    static constexpr int32 ColunasDaGrade = 4;

    /** Abas CHASSI/CLASSE da tela de criacao. */
    UWidget* CriarAbas();

    /** Grade de icones da aba atual (4 colunas). */
    UWidget* CriarGradeDeIcones(TArray<UWidget*>& AlvosDoVeu);

    /** Icone (gerado por codigo) do chassi / da classe, com cache. */
    UTexture2D* IconeDoChassi(FName ChassiId);
    UTexture2D* IconeDaClasse(FName ClassId);

    /** Refaz vitrine, ficha e detalhes com o que esta selecionado agora. */
    void AtualizarSelecao();

    // ---------- Confirmacao ----------
    /** Executa o que a janela de confirmacao estava pedindo. */
    void ExecutarConfirmacao();

    UPROPERTY() TObjectPtr<UVerticalBox> RootBox;
    UPROPERTY() TObjectPtr<UTextBlock> StatusText;
    UPROPERTY() TObjectPtr<UEditableTextBox> AccountBox;
    UPROPERTY() TObjectPtr<UEditableTextBox> PasswordBox;
    UPROPERTY() TObjectPtr<UEditableTextBox> UserNameBox;
    UPROPERTY() TObjectPtr<UEditableTextBox> ConfirmBox;

    /** Nome do personagem (so no passo de escolher a classe, onde a ficha e criada). */
    UPROPERTY() TObjectPtr<UEditableTextBox> NameBox;

    /** O veu de vapor que circula pela tela e acompanha o mouse. */
    UPROPERTY() TObjectPtr<class URunnerVeilWidget> Veil;

    /** Altura da vitrine neste passo (acompanha a tela). */
    float AlturaDaVitrine = 260.f;
    /** Altura da lista de opcoes (a lista rola dentro dela). */
    float AlturaDaLista = 170.f;

    /** Slot do painel: o tamanho e recalculado conforme a tela a cada passo. */
    UPROPERTY() TObjectPtr<class UCanvasPanelSlot> PanelSlotDoPainel;

    /** O "monitor" que mostra o corpo escolhido. */
    UPROPERTY() TObjectPtr<class UImage> PreviewImage;

    /** Coluna da ficha (atributos etc.) que se atualiza sem refazer a tela toda. */
    UPROPERTY() TObjectPtr<class UVerticalBox> InfoBox;

    /** Ator da vitrine 3D (nasce no mundo do menu, fora da vista). */
    UPROPERTY(Transient) TObjectPtr<class ARunnerPreviewActor> Vitrine;

    /** O que a vitrine esta mostrando agora (para nao recarregar a malha a cada quadro). */
    FName ChassiNaVitrine;

    // Estado da janela de confirmacao.
    ERunnerConfirmAction Confirmacao = ERunnerConfirmAction::Nada;
    FString MensagemDaConfirmacao;
    FString IdDaConfirmacao;
    ERunnerMenuStep PassoAntesDaConfirmacao = ERunnerMenuStep::Class;
    UPROPERTY() TObjectPtr<UButton> TertiaryButton;
    UPROPERTY() TObjectPtr<UButton> QuartoButton;

    /** Colunas que se refrescam sem refazer a tela (ficha e detalhes). */
    UPROPERTY() TObjectPtr<class UVerticalBox> FichaBox;
    UPROPERTY() TObjectPtr<class UVerticalBox> DetalhesBox;

    /** Runner marcado na lista de existentes. */
    FString IdPersonagemSelecionado;

    /** Icones gerados (chave = id do chassi/classe). */
    UPROPERTY(Transient) TMap<FName, TObjectPtr<UTexture2D>> IconesDeChassi;
    UPROPERTY(Transient) TMap<FName, TObjectPtr<UTexture2D>> IconesDeClasse;
    UPROPERTY() TArray<TObjectPtr<URunnerOptionButton>> OptionBindings;
    UPROPERTY() TArray<TObjectPtr<URunnerTabButton>> TabBindings;

    ERunnerMenuStep CurrentStep = ERunnerMenuStep::Login;
    FName PendingChassis;
    FName PendingClass;
};
