#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RunnerAnimInstance.generated.h"

UCLASS()
class PLOIDREKRPG_API URunnerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// Locomotion blend (0..1 normalized speed).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation State")
	float Speed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation State")
	bool bIsInAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation State")
	bool bIsShooting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation State")
	bool bIsWallSlide = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation State")
	bool bIsWallJump = false;
};
