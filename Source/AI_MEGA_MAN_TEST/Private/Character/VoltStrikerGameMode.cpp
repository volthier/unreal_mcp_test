#include "Character/VoltStrikerGameMode.h"
#include "Character/VoltStrikerCharacter.h"

AVoltStrikerGameMode::AVoltStrikerGameMode()
{
	DefaultPawnClass = AVoltStrikerCharacter::StaticClass();
}
