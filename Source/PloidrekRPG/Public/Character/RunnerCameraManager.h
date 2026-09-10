#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "RunnerCameraManager.generated.h"

UCLASS()
class PLOIDREKRPG_API ARunnerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ARunnerCameraManager();
};
