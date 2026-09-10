#include "Character/AFGameMode.h"
#include "Character/RunnerCharacter.h"

AAFGameMode::AAFGameMode()
{
	DefaultPawnClass = ARunnerCharacter::StaticClass();
}
