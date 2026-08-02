#pragma once

#include "CoreMinimal.h"
#include "Projectile/VoltStrikerProjectile.h"
#include "VoltStrikerChargeProjectile.generated.h"

/** Large piercing plasma sphere for charge releases. */
UCLASS()
class AI_MEGA_MAN_TEST_API AVoltStrikerChargeProjectile : public AVoltStrikerProjectile
{
	GENERATED_BODY()

public:
	AVoltStrikerChargeProjectile();
};
