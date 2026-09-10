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

protected:
	virtual void BeginPlay() override;
};
