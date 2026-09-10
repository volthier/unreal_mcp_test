#include "Character/MenuGameMode.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Session/RunnerMenuWidget.h"

AMenuGameMode::AMenuGameMode()
{
	// No menu nao existe personagem andando pelo mundo.
	DefaultPawnClass = nullptr;
	PlayerControllerClass = APlayerController::StaticClass();
}

void AMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMenuGameMode: nenhum PlayerController — o menu nao pode abrir."));
		return;
	}

	TSubclassOf<URunnerMenuWidget> WidgetClass = MenuWidgetClass;
	if (!WidgetClass)
	{
		WidgetClass = URunnerMenuWidget::StaticClass();
	}

	URunnerMenuWidget* Menu = CreateWidget<URunnerMenuWidget>(PlayerController, WidgetClass);
	if (!Menu)
	{
		UE_LOG(LogTemp, Error, TEXT("AMenuGameMode: falhou ao criar o widget de menu."));
		return;
	}

	Menu->AddToViewport();

	// Menu e UI pura: cursor visivel e input no widget.
	PlayerController->bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(Menu->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
}
