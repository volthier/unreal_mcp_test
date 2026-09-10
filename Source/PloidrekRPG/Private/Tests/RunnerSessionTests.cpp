#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Data/RunnerCharacterProfile.h"
#include "Session/RunnerSession.h"

/**
 * Fluxo completo sem tela: conta -> login -> chassi -> classe -> ficha -> restauracao.
 * Cobre as regras que o menu apenas chama.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerSessionFlowTest, "Runner.Sessao.ContaECriacao",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerSessionFlowTest::RunTest(const FString& Parameters)
{
    const FString Conta = FString::Printf(TEXT("teste_auto_%d"), FMath::RandRange(100000, 999999));
    const FString Senha = TEXT("senha-de-teste");

    URunnerSession* Sessao = NewObject<URunnerSession>();
    if (!TestNotNull(TEXT("subsystem criado"), Sessao))
    {
        return false;
    }

    FString Erro;

    // Sem login nao se escolhe nada.
    TestFalse(TEXT("nao escolhe chassi antes de entrar"),
        Sessao->SelectChassis(FName(TEXT("Vitaspark")), Erro));

    // Conta
    TestTrue(TEXT("cria conta"), Sessao->CreateAccount(Conta, Senha, Erro));
    TestFalse(TEXT("nao aceita conta repetida"), Sessao->CreateAccount(Conta, Senha, Erro));
    TestFalse(TEXT("recusa senha curta"), Sessao->CreateAccount(Conta + TEXT("b"), TEXT("ab"), Erro));
    TestFalse(TEXT("recusa senha errada"), Sessao->Login(Conta, TEXT("senha-errada"), Erro));
    TestTrue(TEXT("entra com a senha certa"), Sessao->Login(Conta, Senha, Erro));
    TestTrue(TEXT("sessao logada"), Sessao->IsLoggedIn());

    // Selecao: o chassi extinto e o inexistente precisam ser recusados.
    TestFalse(TEXT("recusa os Clyffen (extintos)"), Sessao->SelectChassis(FName(TEXT("Clyffen")), Erro));
    TestFalse(TEXT("recusa chassi inexistente"), Sessao->SelectChassis(FName(TEXT("Inexistente")), Erro));
    TestTrue(TEXT("aceita chassi valido"), Sessao->SelectChassis(FName(TEXT("Vitaspark")), Erro));
    TestFalse(TEXT("recusa classe inexistente"), Sessao->SelectClass(FName(TEXT("Nenhuma")), Erro));
    TestTrue(TEXT("aceita classe valida"), Sessao->SelectClass(FName(TEXT("Blaster")), Erro));
    TestTrue(TEXT("selecao completa"), Sessao->IsSelectionComplete());

    // Ficha: Vitaspark e esguio -> FOR 7, DES 10, CON 9; CA 10; HP = 3*8 + mod(CON 9) = 23.
    FRunnerCharacterProfile Perfil;
    TestTrue(TEXT("cria a ficha"), Sessao->CreateCharacterProfile(Perfil, Erro));
    TestTrue(TEXT("ficha marcada como valida"), Perfil.bValid);
    TestEqual(TEXT("conta gravada na ficha"), Perfil.AccountName, Conta);
    TestEqual(TEXT("FOR do Vitaspark (8-1)"), Perfil.GetAttribute(ERunnerAttribute::Strength), 7);
    TestEqual(TEXT("DES do Vitaspark (8+2)"), Perfil.GetAttribute(ERunnerAttribute::Dexterity), 10);
    TestEqual(TEXT("CON do Vitaspark (8+1)"), Perfil.GetAttribute(ERunnerAttribute::Constitution), 9);
    TestEqual(TEXT("CA com DES 10"), Perfil.ArmorClass, 10);
    TestEqual(TEXT("HP nivel 1 do Blaster d8 com CON 9"), Perfil.MaxHitPoints, 23);
    TestTrue(TEXT("corpo apontado (manequim da engine)"), !Perfil.BodyMesh.IsNull());
    TestTrue(TEXT("sessao tem personagem"), Sessao->HasCharacter());

    // Nova sessao (novo login): o personagem volta da conta.
    URunnerSession* NovaSessao = NewObject<URunnerSession>();
    if (!TestNotNull(TEXT("segunda sessao criada"), NovaSessao))
    {
        return false;
    }

    FString Erro2;
    TestTrue(TEXT("entra na conta de novo"), NovaSessao->Login(Conta, Senha, Erro2));
    TestTrue(TEXT("restaura o personagem"), NovaSessao->RestoreCharacter(Erro2));
    TestEqual(TEXT("chassi restaurado"), NovaSessao->GetSelectedChassis().ToString(), FString(TEXT("Vitaspark")));
    TestEqual(TEXT("classe restaurada"), NovaSessao->GetSelectedClass().ToString(), FString(TEXT("Blaster")));
    TestTrue(TEXT("personagem ativo na nova sessao"), NovaSessao->HasCharacter());
    return true;
}

#endif
