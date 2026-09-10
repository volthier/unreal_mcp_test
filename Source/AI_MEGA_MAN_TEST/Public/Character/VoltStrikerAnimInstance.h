#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "VoltStrikerAnimInstance.generated.h"

UCLASS()
class AI_MEGA_MAN_TEST_API UVoltStrikerAnimInstance : public UAnimInstance
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
