#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RunnerSessionSubsystem.generated.h"

class URunnerSession;

/**
 * Encanamento da engine: existe uma sessao por GameInstance e ela e entregue
 * para a UI e para o GameMode. A regra do fluxo vive em URunnerSession.
 */
UCLASS()
class PLOIDREKRPG_API URunnerSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /** A sessao do jogador (conta, selecao e ficha). */
    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    URunnerSession* GetRunnerSession() const { return Session; }

private:
    UPROPERTY() TObjectPtr<URunnerSession> Session;
};
