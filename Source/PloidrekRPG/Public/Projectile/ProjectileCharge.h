#pragma once

#include "CoreMinimal.h"
#include "Projectile/ProjectileBolt.h"
#include "ProjectileCharge.generated.h"

/** Large piercing plasma sphere for charge releases. */
UCLASS()
class PLOIDREKRPG_API AProjectileCharge : public AProjectileBolt
{
	GENERATED_BODY()

public:
	AProjectileCharge();
};
