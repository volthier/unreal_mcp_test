#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
    Chassis,
    Class
};

/** Ligacao entre um botao de opcao (sem parametros) e o id que ele representa. */
UCLASS()
class PLOIDREKRPG_API URunnerOptionButton : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY() FName OptionId;
    UPROPERTY() TObjectPtr<URunnerMenuWidget> Owner;

    UFUNCTION()
    void HandleClicked();
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

    /** Escolhe o chassi e avanca para a classe. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Menu")
    void SelectChassis(FName ChassisId);

    /** Escolhe a classe. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Menu")
    void SelectClass(FName ClassId);

    /** Passo atual. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Menu")
    void GoToStep(ERunnerMenuStep NewStep);

protected:
    void RebuildLayout();
    void SetStatus(const FString& Message);

    /** Sessao do jogador: a maquina de estado da conta e da criacao. */
    class URunnerSession* GetSession() const;

    UFUNCTION() void OnPrimaryClicked();
    UFUNCTION() void OnSecondaryClicked();

    UPROPERTY() TObjectPtr<UVerticalBox> RootBox;
    UPROPERTY() TObjectPtr<UTextBlock> StatusText;
    UPROPERTY() TObjectPtr<UEditableTextBox> AccountBox;
    UPROPERTY() TObjectPtr<UEditableTextBox> PasswordBox;
    UPROPERTY() TArray<TObjectPtr<URunnerOptionButton>> OptionBindings;

    ERunnerMenuStep CurrentStep = ERunnerMenuStep::Login;
    FName PendingChassis;
    FName PendingClass;
};
