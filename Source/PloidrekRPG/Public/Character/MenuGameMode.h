#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

class URunnerMenuWidget;

/** GameMode do menu: abre a tela de login/criacao. Nao spawna pawn. */
UCLASS()
class PLOIDREKRPG_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMenuGameMode();

	/** Widget de menu. Pode ser trocado por um WBP sem mexer em codigo. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	TSubclassOf<URunnerMenuWidget> MenuWidgetClass;

	/**
	 * Tag do ator de camera que o menu usa como ponto de vista.
	 * O canonico tinha um mapa proprio para o login (MainMenuMap) com a cena atras da UI; a camera vive
	 * nesse mapa e o menu apenas aponta para ela. Sem tag, cai na primeira ACameraActor do nivel.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Menu")
	FName MenuCameraTag = TEXT("MenuCamera");

protected:
	virtual void BeginPlay() override;

	/** Aponta o jogador para a camera do mapa do menu. Devolve false se o mapa nao tiver camera. */
	bool AplicarCameraDoMapa(APlayerController* PlayerController);
};
