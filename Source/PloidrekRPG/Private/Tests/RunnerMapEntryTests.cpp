#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/ConfigCacheIni.h"
#include "Character/MenuGameMode.h"

/**
 * O elo de entrada: o mapa que o JOGO abre precisa apontar para o GameMode do MENU, senao o jogador cai
 * direto no mundo sem passar pelo login.
 *
 * O mapa NAO e escrito aqui na mao: o teste le o GameDefaultMap da config, para nao passar por acidente
 * quando o mapa de entrada muda (foi o caso quando o projeto passou a ter um mapa de login proprio, como
 * no canonico, em vez de abrir o NewMap).
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerMapEntryTest, "Runner.Entrada.MapaApontaParaOMenu",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerMapEntryTest::RunTest(const FString& Parameters)
{
    FString CaminhoConfigurado;
    const bool bAchou = GConfig && GConfig->GetString(TEXT("/Script/Engine.Settings"), TEXT("GameDefaultMap"),
                                                     CaminhoConfigurado, GEngineIni);
    if (!TestTrue(TEXT("a config define GameDefaultMap"), bAchou && !CaminhoConfigurado.IsEmpty()))
    {
        return false;
    }

    // GameDefaultMap vem como /Game/Maps/MainMenuMap.MainMenuMap
    FString CaminhoObjeto = CaminhoConfigurado;
    if (!CaminhoObjeto.Contains(TEXT(".")))
    {
        const FString Nome = FPaths::GetBaseFilename(CaminhoObjeto);
        CaminhoObjeto = CaminhoObjeto + TEXT(".") + Nome;
    }

    UWorld* Mapa = LoadObject<UWorld>(nullptr, *CaminhoObjeto);
    if (!TestNotNull(*FString::Printf(TEXT("mapa de entrada carregou (%s)"), *CaminhoObjeto), Mapa))
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
