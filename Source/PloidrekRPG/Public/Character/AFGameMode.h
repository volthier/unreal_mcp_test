#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AFGameMode.generated.h"

/**
 * GameMode do jogo (nao do menu). Ao entrar o jogador, aplica a ficha escolhida
 * na criacao: o chassi define o corpo (malha) do pawn.
 */
UCLASS()
class PLOIDREKRPG_API AAFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAFGameMode();

protected:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	/** Le a ficha ativa da sessao e aplica no pawn do jogador. */
	void ApplySessionProfileTo(APlayerController* PlayerController);
};
