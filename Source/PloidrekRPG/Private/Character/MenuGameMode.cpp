#include "Character/MenuGameMode.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"
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

	// A cena do mapa (a praca de Kardys, o vestiario dos casulos) e o fundo do menu: o canonico tinha
	// mapa proprio para isso. Se o mapa tem camera, ela vira o ponto de vista do jogador.
	AplicarCameraDoMapa(PlayerController);

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

	UE_LOG(LogTemp, Log, TEXT("AMenuGameMode: menu de entrada aberto (%s)."), *WidgetClass->GetName());

	// Menu e UI pura: cursor visivel e input no widget.
	PlayerController->bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(Menu->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputMode);
}

bool AMenuGameMode::AplicarCameraDoMapa(APlayerController* PlayerController)
{
	UWorld* World = GetWorld();
	if (!World || !PlayerController)
	{
		return false;
	}

	ACameraActor* Camera = nullptr;

	// 1. a camera marcada com a tag tem prioridade
	for (TActorIterator<ACameraActor> It(World); It; ++It)
	{
		if (It->ActorHasTag(MenuCameraTag))
		{
			Camera = *It;
			break;
		}
	}

	// 2. sem tag, a primeira camera do nivel serve (o mapa de login tem uma so)
	if (!Camera)
	{
		// primeira camera do nivel, sem loop: um for com break incondicional e erro de build no UE
		TActorIterator<ACameraActor> Primeira(World);
		if (Primeira)
		{
			Camera = *Primeira;
		}
	}

	if (!Camera)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMenuGameMode: o mapa nao tem ACameraActor — o menu abre sobre o vazio."));
		return false;
	}

	PlayerController->SetViewTargetWithBlend(Camera, 0.f);
	UE_LOG(LogTemp, Log, TEXT("AMenuGameMode: camera do mapa aplicada (%s)."), *Camera->GetActorLabel());
	return true;
}
