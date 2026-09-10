#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Character/MenuGameMode.h"

/**
 * O elo de entrada: o mapa do jogo precisa apontar para o GameMode do MENU,
 * senao o jogador cai direto no mundo sem passar pelo login.
 * Este teste le o WorldSettings do mapa de verdade.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerMapEntryTest, "Runner.Entrada.MapaApontaParaOMenu",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerMapEntryTest::RunTest(const FString& Parameters)
{
    UWorld* Mapa = LoadObject<UWorld>(nullptr, TEXT("/Game/NewMap.NewMap"));
    if (!TestNotNull(TEXT("mapa NewMap carregou"), Mapa))
    {
        return false;
    }

    const AWorldSettings* Settings = Mapa->GetWorldSettings();
    if (!TestNotNull(TEXT("WorldSettings do mapa"), Settings))
    {
        return false;
    }

    UClass* ClasseGameMode = Settings->DefaultGameMode;
    if (!TestNotNull(TEXT("o mapa define um GameMode"), ClasseGameMode))
    {
        return false;
    }

    TestTrue(FString::Printf(TEXT("GameMode do mapa (%s) e o do menu ou deriva dele"), *ClasseGameMode->GetName()),
        ClasseGameMode->IsChildOf(AMenuGameMode::StaticClass()));
    return true;
}

#endif
