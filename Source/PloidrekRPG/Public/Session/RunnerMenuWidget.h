#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/RunnerTypes.h"
#include "RunnerMenuWidget.generated.h"

class UButton;
class UCanvasPanel;
class UEditableTextBox;
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
    Jogar,
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
    void PreencherInfo(UVerticalBox* Caixa, FName ChassisId, FName ClassId);
    /** A vitrine mostra o corpo deste chassi. */
    void MostrarNaVitrine(FName ChassiId);
    /** Termina o ator da vitrine (ao sair do menu). */
    void DestruirVitrine();

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
    UPROPERTY() TArray<TObjectPtr<URunnerOptionButton>> OptionBindings;

    ERunnerMenuStep CurrentStep = ERunnerMenuStep::Login;
    FName PendingChassis;
    FName PendingClass;
};
