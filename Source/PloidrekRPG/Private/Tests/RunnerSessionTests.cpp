#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "Data/RunnerCharacterProfile.h"
#include "Session/RunnerSession.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

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

/**
 * Uma conta pode ter VARIOS Runners: depois do login a lista aparece e o jogador
 * escolhe um existente ou cria um novo. Tambem garante que uma conta nao ve os do outra.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerMultiCharacterTest, "Runner.Sessao.VariosPersonagens",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerMultiCharacterTest::RunTest(const FString& Parameters)
{
    const FString Conta = FString::Printf(TEXT("teste_multi_%d"), FMath::RandRange(100000, 999999));
    const FString Senha = TEXT("senha-de-teste");

    URunnerSession* Sessao = NewObject<URunnerSession>();
    if (!TestNotNull(TEXT("sessao criada"), Sessao))
    {
        return false;
    }

    FString Erro;
    TestTrue(TEXT("cria conta"), Sessao->CreateAccount(Conta, Senha, Erro));
    TestTrue(TEXT("entra na conta"), Sessao->Login(Conta, Senha, Erro));
    TestFalse(TEXT("conta nova nao tem personagem"), Sessao->HasSavedCharacters());
    TestEqual(TEXT("lista vazia no inicio"), Sessao->GetSavedCharacters().Num(), 0);

    // Primeiro Runner
    TestTrue(TEXT("chassi do Runner 1"), Sessao->SelectChassis(FName(TEXT("Vitaspark")), Erro));
    TestTrue(TEXT("classe do Runner 1"), Sessao->SelectClass(FName(TEXT("Blaster")), Erro));
    FRunnerCharacterProfile Perfil1;
    TestTrue(TEXT("cria o Runner 1"), Sessao->CreateCharacterProfile(Perfil1, Erro));

    // Segundo Runner, na mesma conta
    TestTrue(TEXT("chassi do Runner 2"), Sessao->SelectChassis(FName(TEXT("Forgekin")), Erro));
    TestTrue(TEXT("classe do Runner 2"), Sessao->SelectClass(FName(TEXT("Breaker")), Erro));
    FRunnerCharacterProfile Perfil2;
    TestTrue(TEXT("cria o Runner 2"), Sessao->CreateCharacterProfile(Perfil2, Erro));

    const TArray<FRunnerSavedCharacter> Meus = Sessao->GetSavedCharacters();
    TestEqual(TEXT("a conta tem 2 personagens"), Meus.Num(), 2);
    if (Meus.Num() == 2)
    {
        TestEqual(TEXT("chassi do primeiro"), Meus[0].ChassisId.ToString(), FString(TEXT("Vitaspark")));
        TestEqual(TEXT("classe do primeiro"), Meus[0].ClassId.ToString(), FString(TEXT("Blaster")));
        TestEqual(TEXT("chassi do segundo"), Meus[1].ChassisId.ToString(), FString(TEXT("Forgekin")));
        TestEqual(TEXT("classe do segundo"), Meus[1].ClassId.ToString(), FString(TEXT("Breaker")));

        // Escolher um existente carrega a ficha dele
        TestTrue(TEXT("seleciona o primeiro"), Sessao->SelectSavedCharacter(Meus[0].CharacterId, Erro));
        FRunnerCharacterProfile Ativo;
        if (TestTrue(TEXT("tem ficha ativa"), Sessao->GetActiveProfile(Ativo)))
        {
            TestEqual(TEXT("chassi ativo"), Ativo.ChassisId.ToString(), FString(TEXT("Vitaspark")));
            TestEqual(TEXT("HP do Vitaspark/Blaster"), Ativo.MaxHitPoints, 23);
        }

        TestTrue(TEXT("seleciona o segundo"), Sessao->SelectSavedCharacter(Meus[1].CharacterId, Erro));
        Sessao->GetActiveProfile(Ativo);
        TestEqual(TEXT("chassi ativo 2"), Ativo.ChassisId.ToString(), FString(TEXT("Forgekin")));
        TestEqual(TEXT("classe ativa 2"), Ativo.ClassId.ToString(), FString(TEXT("Breaker")));
    }

    TestFalse(TEXT("id inexistente e recusado"), Sessao->SelectSavedCharacter(TEXT("R-99"), Erro));

    // Outra conta nao enxerga estes personagens
    URunnerSession* Outra = NewObject<URunnerSession>();
    const FString Conta2 = Conta + TEXT("b");
    FString Erro2;
    TestTrue(TEXT("cria a segunda conta"), Outra->CreateAccount(Conta2, Senha, Erro2));
    TestTrue(TEXT("entra na segunda conta"), Outra->Login(Conta2, Senha, Erro2));
    TestFalse(TEXT("a segunda conta nao tem personagens"), Outra->HasSavedCharacters());
    TestEqual(TEXT("lista da segunda conta vazia"), Outra->GetSavedCharacters().Num(), 0);
    return true;
}

/**
 * O EOS precisa estar DE PE para o login funcionar. Este teste existe porque a falha era silenciosa:
 * o EOS_Platform_Create exige ClientCredentials completas (ClientId + ClientSecret) e, sem elas, o
 * subsistema sobe sem plataforma e o login cai para o local sem dizer por que. Aqui a suite falha
 * com a instrucao do conserto em vez de deixar isso passar.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerEOSConfigTest, "Runner.Sessao.EOSConfigurado",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerEOSConfigTest::RunTest(const FString& Parameters)
{
    IOnlineSubsystem* Sub = IOnlineSubsystem::Get(FName(TEXT("EOS")));
    if (!Sub)
    {
        AddWarning(TEXT("EOS: subsistema indisponivel (plugin desligado?). O menu cai para o login local."));
        return true;
    }

    IOnlineIdentityPtr Identidade = Sub->GetIdentityInterface();
    if (!Identidade.IsValid())
    {
        AddError(FString::Printf(TEXT("EOS: a plataforma NAO inicializou (servico='%s'). "
            "O EOS_Platform_Create exige ClientId E ClientSecret: confira o bloco [OnlineSubsystemEOS.EOSSettings] "
            "em Config/DefaultEngine.ini e o segredo do artefato DEV em Saved/Config/<Plataforma>/Engine.ini. "
            "Procure por 'ClientSecret cannot be null' no log."), *Sub->GetSubsystemName().ToString()));
        return false;
    }

    AddInfo(FString::Printf(TEXT("EOS de pe: servico='%s'"), *Sub->GetSubsystemName().ToString()));

    URunnerSession* Sessao = NewObject<URunnerSession>();
    if (!TestNotNull(TEXT("sessao criada"), Sessao))
    {
        return false;
    }

    TestTrue(TEXT("a sessao reconhece o EOS como disponivel"), Sessao->IsEOSAvailable());
    TestFalse(TEXT("ninguem logado antes do login"), Sessao->IsLoggedIn());
    TestTrue(TEXT("o nome da conta EOS comeca vazio"), Sessao->GetAccountName().IsEmpty());
    return true;
}

#endif
