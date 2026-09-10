#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Data/RunnerCharacterFactory.h"
#include "Data/RunnerCharacterProfile.h"
#include "Data/RunnerRules.h"
#include "Engine/DataTable.h"

/** Matematica do GDD v3: modificador, proficiencia, cap de capitulo, HP, CA, ataques. */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerRulesMathTest, "Runner.Regras.Matematica",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerRulesMathTest::RunTest(const FString& Parameters)
{
    // GDD 3.2 — modificador = floor((valor - 10) / 2). O 5e arredonda para BAIXO.
    TestEqual(TEXT("mod 7 = -2"), URunnerRules::GetAttributeModifier(7), -2);
    TestEqual(TEXT("mod 8 = -1"), URunnerRules::GetAttributeModifier(8), -1);
    TestEqual(TEXT("mod 10 = 0"), URunnerRules::GetAttributeModifier(10), 0);
    TestEqual(TEXT("mod 15 = +2"), URunnerRules::GetAttributeModifier(15), 2);
    TestEqual(TEXT("mod 20 = +5"), URunnerRules::GetAttributeModifier(20), 5);

    // GDD 3.2 — proficiencia +2 a +6, sem crescer no epico.
    TestEqual(TEXT("prof nivel 1"), URunnerRules::GetProficiencyBonus(1), 2);
    TestEqual(TEXT("prof nivel 5"), URunnerRules::GetProficiencyBonus(5), 3);
    TestEqual(TEXT("prof nivel 20"), URunnerRules::GetProficiencyBonus(20), 6);
    TestEqual(TEXT("prof nivel 40 (epico)"), URunnerRules::GetProficiencyBonus(40), 6);

    // GDD 3.2 — cap por capitulo.
    TestEqual(TEXT("cap capitulo I"), URunnerRules::GetAttributeCapForChapter(1), 20);
    TestEqual(TEXT("cap capitulo II"), URunnerRules::GetAttributeCapForChapter(2), 24);
    TestEqual(TEXT("cap capitulo III"), URunnerRules::GetAttributeCapForChapter(3), 28);

    // GDD 3.4 — ataques por Acao de Ataque, teto 4.
    TestEqual(TEXT("1 ataque no nivel 1"), URunnerRules::GetAttacksPerAttackAction(1), 1);
    TestEqual(TEXT("2 ataques no nivel 5"), URunnerRules::GetAttacksPerAttackAction(5), 2);
    TestEqual(TEXT("3 ataques no nivel 11"), URunnerRules::GetAttacksPerAttackAction(11), 3);
    TestEqual(TEXT("4 ataques no nivel 17"), URunnerRules::GetAttacksPerAttackAction(17), 4);
    TestEqual(TEXT("teto 4 no epico"), URunnerRules::GetAttacksPerAttackAction(40), 4);

    // GDD 3.3 — HP nivel 1 = 3 x dado + mod CON. Breaker d12 com CON 16 => 36 + 3 = 39.
    TestEqual(TEXT("HP nivel 1 Breaker CON 16"), URunnerRules::GetLevelOneHitPoints(12, 16), 39);
    TestEqual(TEXT("HP por nivel Breaker CON 16"), URunnerRules::GetHitPointsPerLevel(12, 16), 15);
    TestEqual(TEXT("HP nivel 1 Blaster CON 14"), URunnerRules::GetLevelOneHitPoints(8, 14), 26);

    // GDD 3.3 — CA = 10 + mod DES + armadura; Eter fixo em 100.
    TestEqual(TEXT("CA com DES 8"), URunnerRules::GetArmorClass(8, 0), 9);
    TestEqual(TEXT("CA com DES 16 e armadura 3"), URunnerRules::GetArmorClass(16, 3), 16);
    TestEqual(TEXT("Eter fixo"), URunnerRules::GetMaxEtherCells(), 100);
    return true;
}

/** O fluxo: chassi + classe -> ficha. Valida as invariantes de TODAS as combinacoes. */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerChassiClasseTest, "Runner.Fluxo.ChassiEClasse",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerChassiClasseTest::RunTest(const FString& Parameters)
{
    UDataTable* Chassis = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Chassis.DT_Chassis"));
    UDataTable* Classes = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_Classes.DT_Classes"));
    if (!TestNotNull(TEXT("DT_Chassis carregou"), Chassis)) { return false; }
    if (!TestNotNull(TEXT("DT_Classes carregou"), Classes)) { return false; }

    const TArray<FName> Selecionaveis = URunnerCharacterFactory::GetChassisIds(Chassis, false);
    const TArray<FName> Todos = URunnerCharacterFactory::GetChassisIds(Chassis, true);
    TestEqual(TEXT("10 chassis selecionaveis (GDD 4)"), Selecionaveis.Num(), 10);
    TestEqual(TEXT("11 linhas contando os Clyffen"), Todos.Num(), 11);
    TestFalse(TEXT("os Clyffen nao sao selecionaveis"),
        URunnerCharacterFactory::IsChassisSelectable(Chassis, FName(TEXT("Clyffen"))));

    const TArray<FName> ClassIds = URunnerCharacterFactory::GetClassIds(Classes);
    TestEqual(TEXT("6 classes base (GDD 5)"), ClassIds.Num(), 6);

    int32 CombinacoesValidas = 0;
    for (const FName& Chassi : Selecionaveis)
    {
        for (const FName& Classe : ClassIds)
        {
            FRunnerCharacterProfile Perfil;
            FString Erro;
            const bool bOk = URunnerCharacterFactory::BuildProfile(
                Chassi, Classe, TEXT("teste"), Chassis, Classes, Perfil, Erro);

            if (!TestTrue(FString::Printf(TEXT("perfil %s + %s [%s]"), *Chassi.ToString(), *Classe.ToString(), *Erro), bOk))
            {
                continue;
            }
            ++CombinacoesValidas;

            int32 Positivos = 0;
            int32 Negativos = 0;
            for (const TPair<ERunnerAttribute, int32>& Par : Perfil.Attributes)
            {
                if (Par.Value > URunnerRules::GetBaseAttributeScore())
                {
                    Positivos += Par.Value - URunnerRules::GetBaseAttributeScore();
                }
                else if (Par.Value < URunnerRules::GetBaseAttributeScore())
                {
                    ++Negativos;
                }
            }
            TestEqual(FString::Printf(TEXT("%s: +3 positivos"), *Chassi.ToString()), Positivos, 3);
            TestEqual(FString::Printf(TEXT("%s: exatamente um -1"), *Chassi.ToString()), Negativos, 1);
            TestTrue(FString::Printf(TEXT("%s: HP positivo"), *Chassi.ToString()), Perfil.MaxHitPoints > 0);
            TestTrue(FString::Printf(TEXT("%s: tem corpo apontado"), *Chassi.ToString()), !Perfil.BodyMesh.IsNull());
        }
    }
    TestEqual(TEXT("60 combinacoes montam ficha"), CombinacoesValidas, 60);

    FRunnerCharacterProfile PerfilClyffen;
    FString ErroClyffen;
    TestFalse(TEXT("chassi extinto e recusado"),
        URunnerCharacterFactory::BuildProfile(FName(TEXT("Clyffen")), FName(TEXT("Blaster")),
            TEXT("teste"), Chassis, Classes, PerfilClyffen, ErroClyffen));
    return true;
}

#endif
