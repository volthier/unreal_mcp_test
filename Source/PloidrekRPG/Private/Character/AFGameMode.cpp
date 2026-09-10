#include "Character/AFGameMode.h"

#include "Character/RunnerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Session/RunnerSession.h"
#include "Session/RunnerSessionSubsystem.h"

AAFGameMode::AAFGameMode()
{
	DefaultPawnClass = ARunnerCharacter::StaticClass();
}

void AAFGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// O pawn so existe depois do Super: por isso a ficha e aplicada aqui, e nao no BeginPlay.
	ApplySessionProfileTo(NewPlayer);
}

void AAFGameMode::ApplySessionProfileTo(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	UGameInstance* GameInstance = GetGameInstance();
	URunnerSessionSubsystem* Subsystem = GameInstance ? GameInstance->GetSubsystem<URunnerSessionSubsystem>() : nullptr;
	URunnerSession* Session = Subsystem ? Subsystem->GetRunnerSession() : nullptr;
	if (!Session || !Session->HasCharacter())
	{
		UE_LOG(LogTemp, Log, TEXT("AFGameMode: sem ficha de criacao na sessao — o pawn fica com o corpo padrao."));
		return;
	}

	FRunnerCharacterProfile Profile;
	if (!Session->GetActiveProfile(Profile))
	{
		return;
	}

	if (ARunnerCharacter* Runner = Cast<ARunnerCharacter>(PlayerController->GetPawn()))
	{
		Runner->ApplyProfile(Profile);
		UE_LOG(LogTemp, Log, TEXT("AFGameMode: ficha aplicada no pawn (chassi %s / classe %s)."),
			*Profile.ChassisId.ToString(), *Profile.ClassId.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AFGameMode: ficha existe mas o pawn nao e um RunnerCharacter."));
	}
}
