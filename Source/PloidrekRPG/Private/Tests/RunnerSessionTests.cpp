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
    const FString Nome = FString::Printf(TEXT("teste_auto_%d"), FMath::RandRange(100000, 999999));
    const FString Conta = Nome + TEXT("@teste.local");
    const FString Senha = TEXT("Senha@Teste1");

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
    TestTrue(TEXT("cria conta"), Sessao->CreateAccount(Conta, Nome, Senha, Erro));
    TestFalse(TEXT("nao aceita conta repetida"), Sessao->CreateAccount(Conta, Nome, Senha, Erro));
    TestFalse(TEXT("recusa senha curta"), Sessao->CreateAccount(Nome + TEXT("b@teste.local"), Nome + TEXT("b"), TEXT("ab"), Erro));
    TestFalse(TEXT("recusa senha errada"), Sessao->Login(Conta, TEXT("Senha@Errada1"), Erro));
    TestTrue(TEXT("entra com a senha certa"), Sessao->Login(Conta, Senha, Erro));
    TestTrue(TEXT("sessao logada"), Sessao->IsLoggedIn());

    // Selecao: o chassi extinto e o inexistente precisam ser recusados.
    TestFalse(TEXT("recusa os Clyffen (extintos)"), Sessao->SelectChassis(FName(TEXT("Clyffen")), Erro));
    TestFalse(TEXT("recusa chassi inexistente"), Sessao->SelectChassis(FName(TEXT("Inexistente")), Erro));
    TestTrue(TEXT("aceita chassi valido"), Sessao->SelectChassis(FName(TEXT("Vitaspark")), Erro));
    TestFalse(TEXT("recusa classe inexistente"), Sessao->SelectClass(FName(TEXT("Nenhuma")), Erro));
    TestTrue(TEXT("aceita classe valida"), Sessao->SelectClass(FName(TEXT("Blaster")), Erro));

    // O nome do personagem e obrigatorio e tem regra propria (ver Runner.Sessao.RegrasDeConta).
    {
        FRunnerCharacterProfile SemNome;
        TestFalse(TEXT("sem nome nao cria a ficha"), Sessao->CreateCharacterProfile(SemNome, Erro));
    }
    Sessao->SetPendingCharacterName(TEXT("Volt"));
    TestTrue(TEXT("selecao completa"), Sessao->IsSelectionComplete());

    // Ficha: Vitaspark e esguio -> FOR 7, DES 10, CON 9; CA 10; HP = 3*8 + mod(CON 9) = 23.
    FRunnerCharacterProfile Perfil;
    TestTrue(TEXT("cria a ficha"), Sessao->CreateCharacterProfile(Perfil, Erro));
    TestTrue(TEXT("ficha marcada como valida"), Perfil.bValid);
    TestEqual(TEXT("nome de usuario gravado na ficha"), Perfil.AccountName, Nome);
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
    const FString Nome = FString::Printf(TEXT("teste_multi_%d"), FMath::RandRange(100000, 999999));
    const FString Conta = Nome + TEXT("@teste.local");
    const FString Senha = TEXT("Senha@Teste1");

    URunnerSession* Sessao = NewObject<URunnerSession>();
    if (!TestNotNull(TEXT("sessao criada"), Sessao))
    {
        return false;
    }

    FString Erro;
    TestTrue(TEXT("cria conta"), Sessao->CreateAccount(Conta, Nome, Senha, Erro));
    TestTrue(TEXT("entra na conta"), Sessao->Login(Conta, Senha, Erro));
    TestFalse(TEXT("conta nova nao tem personagem"), Sessao->HasSavedCharacters());
    TestEqual(TEXT("lista vazia no inicio"), Sessao->GetSavedCharacters().Num(), 0);

    // Primeiro Runner
    TestTrue(TEXT("chassi do Runner 1"), Sessao->SelectChassis(FName(TEXT("Vitaspark")), Erro));
    TestTrue(TEXT("classe do Runner 1"), Sessao->SelectClass(FName(TEXT("Blaster")), Erro));
    Sessao->SetPendingCharacterName(TEXT("Volt"));
    FRunnerCharacterProfile Perfil1;
    TestTrue(TEXT("cria o Runner 1"), Sessao->CreateCharacterProfile(Perfil1, Erro));
    TestEqual(TEXT("o nome vai para a ficha"), Perfil1.CharacterName, FString(TEXT("Volt")));

    // Segundo Runner, na mesma conta
    TestTrue(TEXT("chassi do Runner 2"), Sessao->SelectChassis(FName(TEXT("Forgekin")), Erro));
    TestTrue(TEXT("classe do Runner 2"), Sessao->SelectClass(FName(TEXT("Breaker")), Erro));
    Sessao->SetPendingCharacterName(TEXT("Brasa"));
    FRunnerCharacterProfile Perfil2;
    TestTrue(TEXT("cria o Runner 2"), Sessao->CreateCharacterProfile(Perfil2, Erro));

    // O nome e unico dentro da conta.
    Sessao->SetPendingCharacterName(TEXT("Volt"));
    TestFalse(TEXT("recusa nome repetido na conta"), Sessao->CreateCharacterProfile(Perfil2, Erro));

    const TArray<FRunnerSavedCharacter> Meus = Sessao->GetSavedCharacters();
    TestEqual(TEXT("a conta tem 2 personagens"), Meus.Num(), 2);
    if (Meus.Num() == 2)
    {
        TestEqual(TEXT("o nome aparece na lista"), Meus[0].GetDisplayName(), FString(TEXT("Volt")));
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

    // Apagar personagem: a lista encolhe, o id e o nome voltam a ficar livres, e o que nao existe e recusado.
    TestFalse(TEXT("id inexistente nao apaga"), Sessao->DeleteSavedCharacter(TEXT("R-99"), Erro));
    if (Meus.Num() == 2)
    {
        const FString IdApagado = Meus[0].CharacterId;
        TestTrue(TEXT("apaga o primeiro"), Sessao->DeleteSavedCharacter(IdApagado, Erro));
        TestEqual(TEXT("sobrou um personagem"), Sessao->GetSavedCharacters().Num(), 1);
        TestFalse(TEXT("apagar de novo o mesmo falha"), Sessao->DeleteSavedCharacter(IdApagado, Erro));

        // O id e o nome do apagado voltam a ser livres.
        Sessao->SelectChassis(FName(TEXT("Vitaspark")), Erro);
        Sessao->SelectClass(FName(TEXT("Blaster")), Erro);
        Sessao->SetPendingCharacterName(TEXT("Volt"));
        FRunnerCharacterProfile Terceiro;
        TestTrue(TEXT("cria de novo com o nome e o id livres"), Sessao->CreateCharacterProfile(Terceiro, Erro));

        const TArray<FRunnerSavedCharacter> Depois = Sessao->GetSavedCharacters();
        TestEqual(TEXT("voltou a dois personagens"), Depois.Num(), 2);
        const bool bReaproveitou = Depois.ContainsByPredicate([&IdApagado](const FRunnerSavedCharacter& S)
            { return S.CharacterId == IdApagado; });
        TestTrue(TEXT("o id do apagado foi reaproveitado"), bReaproveitou);
    }

    // Outra conta nao enxerga estes personagens
    URunnerSession* Outra = NewObject<URunnerSession>();
    const FString Nome2 = Nome + TEXT("b");
    const FString Conta2 = Nome2 + TEXT("@teste.local");
    FString Erro2;
    TestTrue(TEXT("cria a segunda conta"), Outra->CreateAccount(Conta2, Nome2, Senha, Erro2));
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

    // Dev Auth com os campos em branco: a sessao barra ANTES de incomodar o SDK. Sem isso o EOS
    // devolvia EOS_InvalidParameters ('EOS_Auth_Credentials.Id must not be null or empty').
    FString ErroDevAuth;
    TestFalse(TEXT("Dev Auth sem host e sem credencial e recusado"),
        Sessao->LoginWithEOS(TEXT("developer"), TEXT(""), TEXT(""), ErroDevAuth));
    TestTrue(TEXT("e a mensagem diz o que digitar"), ErroDevAuth.Contains(TEXT("host:porta")));
    TestFalse(TEXT("Dev Auth so com o host tambem e recusado"),
        Sessao->LoginWithEOS(TEXT("developer"), TEXT("localhost:8081"), TEXT("  "), ErroDevAuth));
    return true;
}

/**
 * A regra da tela de criar conta, caso a caso — e o texto que o jogador le quando erra.
 * Nasceu do pedido de ter os campos obrigatorios (e-mail, nome de usuario, senha e confirmacao),
 * com a senha exigindo 8+ caracteres, maiuscula, minuscula e caractere especial.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerAccountRulesTest, "Runner.Sessao.RegrasDeConta",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerAccountRulesTest::RunTest(const FString& Parameters)
{
    FString Erro;

    // O caso bom.
    TestTrue(TEXT("conta valida passa"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek.gg"), TEXT("volt_striker"), TEXT("Senha@Forte1"), TEXT("Senha@Forte1"), Erro));

    // E-mail
    TestFalse(TEXT("sem e-mail"), URunnerSession::ValidateNewAccount(TEXT(""), TEXT("volt_striker"), TEXT("Senha@Forte1"), TEXT("Senha@Forte1"), Erro));
    TestTrue(TEXT("o erro fala de e-mail"), Erro.Contains(TEXT("e-mail")));
    TestFalse(TEXT("e-mail sem arroba"), URunnerSession::ValidateNewAccount(TEXT("volt.gg"), TEXT("volt_striker"), TEXT("Senha@Forte1"), TEXT("Senha@Forte1"), Erro));
    TestFalse(TEXT("e-mail sem dominio com ponto"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek"), TEXT("volt_striker"), TEXT("Senha@Forte1"), TEXT("Senha@Forte1"), Erro));
    TestFalse(TEXT("e-mail com espaco"), URunnerSession::ValidateNewAccount(TEXT("vo lt@ploidrek.gg"), TEXT("volt_striker"), TEXT("Senha@Forte1"), TEXT("Senha@Forte1"), Erro));

    // Nome de usuario
    TestFalse(TEXT("nome de usuario curto"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek.gg"), TEXT("vo"), TEXT("Senha@Forte1"), TEXT("Senha@Forte1"), Erro));
    TestTrue(TEXT("o erro fala de nome de usuario"), Erro.Contains(TEXT("nome de usuario")));
    TestFalse(TEXT("nome de usuario com espaco"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek.gg"), TEXT("volt striker"), TEXT("Senha@Forte1"), TEXT("Senha@Forte1"), Erro));

    // Senha: 8+, maiuscula, minuscula, especial
    TestFalse(TEXT("senha com 7 caracteres"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek.gg"), TEXT("volt_striker"), TEXT("S@nha12"), TEXT("S@nha12"), Erro));
    TestTrue(TEXT("o erro fala de 8 caracteres"), Erro.Contains(TEXT("8 caracteres")));
    TestFalse(TEXT("senha sem maiuscula"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek.gg"), TEXT("volt_striker"), TEXT("senha@forte1"), TEXT("senha@forte1"), Erro));
    TestTrue(TEXT("o erro fala de maiuscula"), Erro.Contains(TEXT("maiuscula")));
    TestFalse(TEXT("senha sem minuscula"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek.gg"), TEXT("volt_striker"), TEXT("SENHA@FORTE1"), TEXT("SENHA@FORTE1"), Erro));
    TestTrue(TEXT("o erro fala de minuscula"), Erro.Contains(TEXT("minuscula")));
    TestFalse(TEXT("senha sem caractere especial"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek.gg"), TEXT("volt_striker"), TEXT("SenhaForte1"), TEXT("SenhaForte1"), Erro));
    TestTrue(TEXT("o erro fala de caractere especial"), Erro.Contains(TEXT("caractere especial")));
    TestFalse(TEXT("confirmacao diferente"), URunnerSession::ValidateNewAccount(TEXT("volt@ploidrek.gg"), TEXT("volt_striker"), TEXT("Senha@Forte1"), TEXT("Senha@Forte2"), Erro));
    TestTrue(TEXT("o erro fala da confirmacao"), Erro.Contains(TEXT("confirmacao")));

    // Na sessao: o e-mail e a identidade, o nome de usuario e o que aparece.
    const FString Nome = FString::Printf(TEXT("regras_%d"), FMath::RandRange(100000, 999999));
    const FString Conta = Nome + TEXT("@teste.local");
    const FString Senha = TEXT("Senha@Forte1");

    URunnerSession* Sessao = NewObject<URunnerSession>();
    TestTrue(TEXT("cria a conta pela regra"), Sessao->CreateAccount(Conta, Nome, Senha, Erro));
    TestFalse(TEXT("recusa o mesmo e-mail"), Sessao->CreateAccount(Conta, Nome + TEXT("2"), Senha, Erro));
    TestFalse(TEXT("recusa o mesmo nome de usuario"), Sessao->CreateAccount(Nome + TEXT("3@teste.local"), Nome, Senha, Erro));
    TestTrue(TEXT("entra pelo e-mail"), Sessao->Login(Conta, Senha, Erro));
    TestEqual(TEXT("o e-mail fica guardado"), Sessao->GetAccountEmail().ToLower(), Conta.ToLower());
    TestEqual(TEXT("o nome de usuario e o que aparece"), Sessao->GetAccountName(), Nome);
    TestFalse(TEXT("o nome de usuario nao serve de login"), Sessao->Login(Nome, Senha, Erro));
    return true;
}

#endif
