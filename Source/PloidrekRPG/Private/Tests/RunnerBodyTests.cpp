#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Character/RunnerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/RunnerCharacterFactory.h"
#include "Data/RunnerCharacterProfile.h"
#include "Data/RunnerRules.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

/**
 * O corpo do personagem: spawna um Runner de verdade num mundo, aplica a ficha
 * e confere que a MALHA e a ANIMACAO entraram — e que o estado muda com o movimento.
 * E o mais perto de "usar o boneco da engine" que da para provar sem tela.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerBodyAndAnimationTest, "Runner.Corpo.MalhaEAnimacao",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerBodyAndAnimationTest::RunTest(const FString& Parameters)
{
    if (!GEngine)
    {
        return false;
    }

    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false, TEXT("RunnerBodyTestWorld"));
    if (!TestNotNull(TEXT("mundo de teste criado"), World))
    {
        return false;
    }

    FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
    Context.SetCurrentWorld(World);
    World->InitializeActorsForPlay(FURL());

    // Ficha real: chassi Vitaspark + classe Blaster, montada pela fabrica do jogo.
    UDataTable* Chassis = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Chassis.DT_Chassis"));
    UDataTable* Classes = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Classes.DT_Classes"));
    FRunnerCharacterProfile Perfil;
    FString Erro;

    ARunnerCharacter* Runner = nullptr;
    if (Chassis && Classes &&
        URunnerCharacterFactory::BuildProfile(FName(TEXT("Vitaspark")), FName(TEXT("Blaster")),
            TEXT("teste"), Chassis, Classes, Perfil, Erro))
    {
        Runner = World->SpawnActor<ARunnerCharacter>();
    }

    if (TestNotNull(TEXT("Runner spawnado no mundo"), Runner))
    {
        Runner->ApplyProfile(Perfil);

        USkeletalMeshComponent* Mesh = Runner->GetMesh();
        if (TestNotNull(TEXT("componente de malha"), Mesh))
        {
            TestNotNull(TEXT("malha do chassi aplicada"), Mesh->GetSkeletalMeshAsset());
            TestEqual(TEXT("modo de animacao unica"),
                static_cast<int32>(Mesh->GetAnimationMode()), static_cast<int32>(EAnimationMode::AnimationSingleNode));
            TestEqual(TEXT("parado no inicio"),
                static_cast<int32>(Runner->LocomotionState), static_cast<int32>(ERunnerLocomotion::Idle));
            TestNotNull(TEXT("corpo tem esqueleto (articulacoes)"), Mesh->GetSkeletalMeshAsset()->GetSkeleton());
        }

        // Movimento muda o estado do corpo.
        if (UCharacterMovementComponent* Movement = Runner->GetCharacterMovement())
        {
            Movement->Velocity = FVector(200.f, 0.f, 0.f);
            Runner->UpdateLocomotionAnimation();
            TestEqual(TEXT("andando a 200"),
                static_cast<int32>(Runner->LocomotionState), static_cast<int32>(ERunnerLocomotion::Walk));

            Movement->Velocity = FVector(500.f, 0.f, 0.f);
            Runner->UpdateLocomotionAnimation();
            TestEqual(TEXT("correndo a 500"),
                static_cast<int32>(Runner->LocomotionState), static_cast<int32>(ERunnerLocomotion::Run));

            Movement->Velocity = FVector::ZeroVector;
            Runner->UpdateLocomotionAnimation();
            TestEqual(TEXT("volta a parado"),
                static_cast<int32>(Runner->LocomotionState), static_cast<int32>(ERunnerLocomotion::Idle));
        }
    }

    GEngine->DestroyWorldContext(World);
    World->DestroyWorld(false);
    return true;
}

/** A decisao de locomocao e pura: testa os limiares sem mundo nenhum. */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerLocomotionRulesTest, "Runner.Regras.Locomocao",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerLocomotionRulesTest::RunTest(const FString& Parameters)
{
    TestEqual(TEXT("parado com velocidade 0"),
        static_cast<int32>(URunnerRules::GetLocomotionState(0.f, false)), static_cast<int32>(ERunnerLocomotion::Idle));
    TestEqual(TEXT("andando a 100"),
        static_cast<int32>(URunnerRules::GetLocomotionState(100.f, false)), static_cast<int32>(ERunnerLocomotion::Walk));
    TestEqual(TEXT("correndo a 400"),
        static_cast<int32>(URunnerRules::GetLocomotionState(400.f, false)), static_cast<int32>(ERunnerLocomotion::Run));
    TestEqual(TEXT("no ar mesmo parado"),
        static_cast<int32>(URunnerRules::GetLocomotionState(0.f, true)), static_cast<int32>(ERunnerLocomotion::Jump));
    TestEqual(TEXT("no ar tem prioridade sobre a velocidade"),
        static_cast<int32>(URunnerRules::GetLocomotionState(600.f, true)), static_cast<int32>(ERunnerLocomotion::Jump));
    return true;
}

#endif
