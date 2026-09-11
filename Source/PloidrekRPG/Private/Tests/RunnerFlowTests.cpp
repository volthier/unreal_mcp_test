#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"

#include "Session/RunnerMenuWidget.h"

#include "Engine/AssetManager.h"

#include "Data/RunnerCharacterFactory.h"
#include "Data/RunnerCharacterProfile.h"
#include "Data/RunnerRules.h"
#include "Character/AFGameMode.h"
#include "Data/RunnerChassisData.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "Data/RunnerGameSettings.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceConstant.h"
#include "NiagaraSystem.h"

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


#include "UI/RunnerIconFactory.h"
#include "Engine/Texture2D.h"


/**
 * Os icones da criacao sao desenhados por codigo. O teste garante que o desenho realmente sai
 * (nao e textura vazia nem transparente) e que cada tipo de corpo/papel cai no glifo certo.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerIconTest, "Runner.UI.Icones",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerIconTest::RunTest(const FString& Parameters)
{
    struct FCaso
    {
        ERunnerIconGlyph Glifo;
        const TCHAR* Nome;
    };

    const FCaso Casos[] = {
        { ERunnerIconGlyph::Triangulo, TEXT("triangulo") },
        { ERunnerIconGlyph::Quadrado,  TEXT("quadrado") },
        { ERunnerIconGlyph::Losango,   TEXT("losango") },
        { ERunnerIconGlyph::Cruz,      TEXT("cruz") },
        { ERunnerIconGlyph::Anel,      TEXT("anel") },
        { ERunnerIconGlyph::Mais,      TEXT("mais") },
        { ERunnerIconGlyph::Escudo,    TEXT("escudo") },
    };

    constexpr int32 Lado = 64;
    for (const FCaso& Caso : Casos)
    {
        UTexture2D* Icone = RunnerIcons::CriarIcone(GetTransientPackage(), Caso.Glifo,
                                                    FLinearColor(0.85f, 0.25f, 0.15f), Lado);
        if (!TestNotNull(*FString::Printf(TEXT("icone %s criado"), Caso.Nome), Icone))
        {
            continue;
        }

        TestEqual(*FString::Printf(TEXT("lado do icone %s"), Caso.Nome), Icone->GetSizeX(), Lado);

        // Conta os pixels que aparecem: a placa ocupa boa parte do icone.
        int32 Visiveis = 0;
        FTexture2DMipMap& Mip = Icone->GetPlatformData()->Mips[0];
        const uint8* Dados = static_cast<const uint8*>(Mip.BulkData.Lock(LOCK_READ_ONLY));
        for (int32 Pixel = 0; Pixel < Lado * Lado; ++Pixel)
        {
            if (Dados[Pixel * 4 + 3] > 8)
            {
                ++Visiveis;
            }
        }
        Mip.BulkData.Unlock();

        TestTrue(*FString::Printf(TEXT("o icone %s desenhou algo"), Caso.Nome), Visiveis > 400);
    }

    // Cada tipo de corpo e cada papel precisam cair no glifo que os representa.
    TestTrue(TEXT("corpo esguio e triangulo"), RunnerIcons::GlifoDoCorpo(ERunnerBodyType::Slender) == ERunnerIconGlyph::Triangulo);
    TestTrue(TEXT("corpo truncoso e quadrado"), RunnerIcons::GlifoDoCorpo(ERunnerBodyType::Stocky) == ERunnerIconGlyph::Quadrado);
    TestTrue(TEXT("corpo fragil e losango"), RunnerIcons::GlifoDoCorpo(ERunnerBodyType::Fragile) == ERunnerIconGlyph::Losango);
    TestTrue(TEXT("tank e escudo"), RunnerIcons::GlifoDoPapel(ERunnerRole::Tank) == ERunnerIconGlyph::Escudo);
    TestTrue(TEXT("suporte e mais"), RunnerIcons::GlifoDoPapel(ERunnerRole::Support) == ERunnerIconGlyph::Mais);
    TestTrue(TEXT("dps e cruz"), RunnerIcons::GlifoDoPapel(ERunnerRole::DPS) == ERunnerIconGlyph::Cruz);
    return true;
}


/**
 * O plugin Game Features cobra uma regra de Asset Manager para GameFeatureData ao abrir o editor
 * ("Asset Manager settings do not include an entry for assets of type GameFeatureData"). Sem ela o
 * log abre com erro em LoadErrors, mesmo sem o projeto usar game features.
 *
 * Este teste consulta EXATAMENTE a mesma condicao que o plugin verifica (a regra padrao do tipo),
 * entao ele quebra se alguem remover a entrada de Config/DefaultGame.ini.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerAssetManagerRuleTest, "Runner.Config.RegraDeGameFeatureData",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerAssetManagerRuleTest::RunTest(const FString& Parameters)
{
    const FPrimaryAssetId IdDoTipo(FPrimaryAssetType(TEXT("GameFeatureData")), NAME_None);
    const FPrimaryAssetRules Regras = UAssetManager::Get().GetPrimaryAssetRules(IdDoTipo);

    TestFalse(TEXT("o Config declara a regra de GameFeatureData (Config/DefaultGame.ini)"), Regras.IsDefault());
    return true;
}


/**
 * A tabela de navegacao dos botoes. Nasceu de um bug de verdade: o botao "Criar novo" na lista de
 * Runners continuava fazendo logout e mandava o jogador para o login em vez da criacao (o rotulo
 * tinha mudado e a navegacao nao). Sendo funcao pura, agora isso tem teste.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerMenuRoutingTest, "Runner.UI.NavegacaoDosBotoes",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerMenuRoutingTest::RunTest(const FString& Parameters)
{
    // Lista de Runners: Jogar nao navega, Criar novo vai para a criacao, Voltar sai para o login.
    TestTrue(TEXT("Jogar fica na lista"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Principal, ERunnerMenuStep::CharacterSelect, true) == ERunnerMenuStep::CharacterSelect);
    TestTrue(TEXT("Criar novo leva para a criacao (era o bug: ia para o login)"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Secundario, ERunnerMenuStep::CharacterSelect, true) == ERunnerMenuStep::Chassis);
    TestTrue(TEXT("Voltar sai para o login"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Quarto, ERunnerMenuStep::CharacterSelect, true) == ERunnerMenuStep::Login);

    // Criacao: Voltar volta para a lista quando a conta tem Runner, senao sai.
    TestTrue(TEXT("Voltar com Runner volta para a lista"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Secundario, ERunnerMenuStep::Chassis, true) == ERunnerMenuStep::CharacterSelect);
    TestTrue(TEXT("Voltar sem Runner sai para o login"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Secundario, ERunnerMenuStep::Chassis, false) == ERunnerMenuStep::Login);
    TestTrue(TEXT("Voltar ao chassi na aba da classe"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Secundario, ERunnerMenuStep::Class, true) == ERunnerMenuStep::Chassis);

    // Criar personagem leva da aba do chassi para a da classe; na classe ele cria (nao navega).
    TestTrue(TEXT("Criar personagem vai para a aba da classe"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Principal, ERunnerMenuStep::Chassis, false) == ERunnerMenuStep::Class);
    TestTrue(TEXT("Criar personagem na aba da classe fica"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Principal, ERunnerMenuStep::Class, false) == ERunnerMenuStep::Class);

    // Login: o terceiro botao abre a criacao de conta local.
    TestTrue(TEXT("criar conta local sai do login"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Terceiro, ERunnerMenuStep::Login, false) == ERunnerMenuStep::CreateAccount);
    TestTrue(TEXT("voltar da criacao de conta"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Secundario, ERunnerMenuStep::CreateAccount, false) == ERunnerMenuStep::Login);

    // Excluir na lista abre a confirmacao: nao navega sozinho.
    TestTrue(TEXT("Excluir nao navega sozinho"),
        RunnerDestinoDoBotao(ERunnerMenuButton::Terceiro, ERunnerMenuStep::CharacterSelect, true) == ERunnerMenuStep::CharacterSelect);
    return true;
}


/**
 * O nucleo: o chassi E o cristal, e cada chassi tem a SUA cor (Docs/Prompts_Arte_Ploidrek.md 9b).
 * Este teste amarra os tres elos: dado (CoreColor na DataTable) -> material (MI_Nucleo_<Chassi>) ->
 * configuracao (malha e efeito do nucleo). Se alguem trocar a cor no CSV e esquecer de gerar a instancia,
 * o teste acusa - que foi exatamente o risco quando as cores passaram a ser semanticas.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerNucleoTest, "Runner.Nucleo.CoresEInstancias",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerNucleoTest::RunTest(const FString& Parameters)
{
    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    if (!TestNotNull(TEXT("configuracao do Runner"), Ajustes))
    {
        return false;
    }

    // 1. a malha do cristal existe e carrega
    UStaticMesh* Malha = Ajustes->NucleoMesh.LoadSynchronous();
    TestNotNull(TEXT("malha do nucleo configurada"), Malha);

    // 2. o efeito da aura existe (a nuvem de gelo em volta do cristal)
    // A aura e uma casca de material aditivo com a textura de geada (tingida com a cor do chassi). O Niagara
    // e opcional: se estiver configurado, tem de carregar; se nao estiver, o teste nao reclama.
    if (!Ajustes->MaterialDaAura.IsNull())
    {
        TestNotNull(TEXT("material da aura carrega"), Ajustes->MaterialDaAura.LoadSynchronous());
    }
    if (!Ajustes->AuraDoNucleo.IsNull())
    {
        TestNotNull(TEXT("efeito Niagara da aura carrega"), Ajustes->AuraDoNucleo.LoadSynchronous());
    }

    // 3. cada chassi da tabela tem cor propria E a instancia de material correspondente
    const UDataTable* Tabela = Ajustes->ChassisTable.LoadSynchronous();
    if (!TestNotNull(TEXT("DT_Chassis"), Tabela))
    {
        return false;
    }

    TArray<FName> Ids = URunnerCharacterFactory::GetChassisIds(Tabela, /*bIncludeExtinct=*/true);
    TestTrue(TEXT("a tabela tem chassis"), Ids.Num() > 0);

    int32 ComCorPropria = 0;
    for (const FName& Id : Ids)
    {
        const FRunnerChassisData* Linha = Tabela->FindRow<FRunnerChassisData>(Id, TEXT("RunnerNucleoTest"), false);
        if (!Linha)
        {
            AddError(FString::Printf(TEXT("linha ausente para %s"), *Id.ToString()));
            continue;
        }

        // A cor do nucleo nao pode ser o branco padrao: ela diz o que o chassi FAZ.
        const FLinearColor Cor = Linha->CoreColor;
        const bool bTemCor = !(FMath::IsNearlyEqual(Cor.R, 1.f) && FMath::IsNearlyEqual(Cor.G, 1.f) && FMath::IsNearlyEqual(Cor.B, 1.f));
        TestTrue(FString::Printf(TEXT("%s tem cor de nucleo propria"), *Id.ToString()), bTemCor);
        if (bTemCor)
        {
            ++ComCorPropria;
        }

        // A instancia do material existe e bate com a cor do dado (dado -> material, sem divergencia).
        const FString NomeDoMaterial = FString::Printf(TEXT("MI_Nucleo_%s"), *Id.ToString());
        const FString Caminho = FString::Printf(TEXT("%s/%s.%s"), *Ajustes->PastaDosMateriaisDoNucleo, *NomeDoMaterial, *NomeDoMaterial);
        UMaterialInstanceConstant* Instancia = LoadObject<UMaterialInstanceConstant>(nullptr, *Caminho);
        if (!TestNotNull(*FString::Printf(TEXT("instancia de material de %s"), *Id.ToString()), Instancia))
        {
            continue;
        }

        FLinearColor CorDoMaterial;
        TestTrue(*FString::Printf(TEXT("instancia de %s tem CorDoNucleo"), *Id.ToString()),
            Instancia->GetVectorParameterValue(FMaterialParameterInfo(TEXT("CorDoNucleo")), CorDoMaterial));
        TestTrue(*FString::Printf(TEXT("material de %s bate com a DataTable"), *Id.ToString()),
            FMath::IsNearlyEqual(CorDoMaterial.R, Cor.R, 0.02f)
                && FMath::IsNearlyEqual(CorDoMaterial.G, Cor.G, 0.02f)
                && FMath::IsNearlyEqual(CorDoMaterial.B, Cor.B, 0.02f));
    }

    TestTrue(TEXT("todos os chassis tem cor de nucleo propria"), ComCorPropria == Ids.Num());
    return true;
}


/**
 * O mundo aberto: o botao Jogar leva para um mapa proprio, que se declara em AFGameMode e tem onde o
 * jogador nascer. E o elo que faltava entre a tela de selecao e o jogo.
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerMundoTest, "Runner.Mundo.AreaInicial",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerMundoTest::RunTest(const FString& Parameters)
{
    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    if (!TestNotNull(TEXT("configuracao do Runner"), Ajustes))
    {
        return false;
    }

    const FString Caminho = Ajustes->MapaDoMundo;
    TestFalse(TEXT("o mapa do mundo esta configurado"), Caminho.IsEmpty());

    const FString CaminhoObjeto = Caminho.Contains(TEXT("."))
        ? Caminho
        : Caminho + TEXT(".") + FPaths::GetBaseFilename(Caminho);
    UWorld* Mundo = LoadObject<UWorld>(nullptr, *CaminhoObjeto);
    if (!TestNotNull(*FString::Printf(TEXT("o mapa do mundo carrega (%s)"), *CaminhoObjeto), Mundo))
    {
        return false;
    }

    const AWorldSettings* Settings = Mundo->GetWorldSettings();
    if (!TestNotNull(TEXT("WorldSettings do mundo"), Settings))
    {
        return false;
    }
    UClass* Classe = Settings->DefaultGameMode;
    if (!TestNotNull(TEXT("o mundo define um GameMode"), Classe))
    {
        return false;
    }
    TestTrue(FString::Printf(TEXT("GameMode do mundo (%s) e o de jogo"), *Classe->GetName()),
        Classe->IsChildOf(AAFGameMode::StaticClass()));

    // Conta sem break: um for com break incondicional e erro de build no UE (-Wunreachable-code-loop-increment).
    int32 QuantosInicios = 0;
    for (TActorIterator<APlayerStart> It(Mundo); It; ++It)
    {
        ++QuantosInicios;
    }
    TestTrue(TEXT("o mundo tem PlayerStart"), QuantosInicios > 0);

    // A cidade e procedimental: o grafo de PCG precisa existir para a area inicial ser gerada.
    UObject* Grafo = LoadObject<UObject>(nullptr, TEXT("/Game/AI_Assets/pcg/PCG_CidadeKardys.PCG_CidadeKardys"));
    TestNotNull(TEXT("o grafo de PCG da cidade existe"), Grafo);
    return true;
}

// ---------------------------------------------------------------------------------------------------------
// KIT VISUAL do menu (fase AAA).
//
// Por que este teste existe: na rodada 13 descobrimos que a configuracao do fundo apontava para uma textura
// que NUNCA tinha sido importada - a tela estava configurada para um asset inexistente e ninguem percebeu,
// porque configuracao errada nao quebra build. Aqui ela quebra teste.
// ---------------------------------------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunnerKitVisualTest, "Runner.UI.KitVisual",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRunnerKitVisualTest::RunTest(const FString& Parameters)
{
    const URunnerGameSettings* Ajustes = GetDefault<URunnerGameSettings>();
    if (!TestNotNull(TEXT("RunnerGameSettings existe"), Ajustes))
    {
        return false;
    }

    // A checagem e de PACOTE, nao de carregamento: existe em qualquer modo (inclusive commandlet sem RHI) e e
    // exatamente o defeito que aconteceu de verdade - a config apontando para um asset que nunca foi importado.
    auto PacoteExiste = [this](const TCHAR* Rotulo, const TSoftObjectPtr<UTexture2D>& Alvo) -> bool
    {
        if (!TestFalse(FString::Printf(TEXT("a config aponta %s"), Rotulo), Alvo.IsNull()))
        {
            return false;
        }
        const FString Pacote = Alvo.ToSoftObjectPath().GetLongPackageName();
        return TestTrue(FString::Printf(TEXT("%s existe no disco (%s)"), Rotulo, *Pacote),
            FPackageName::DoesPackageExist(Pacote));
    };

    PacoteExiste(TEXT("o fundo do login"), Ajustes->TexturaDoFundo);
    PacoteExiste(TEXT("a moldura do painel"), Ajustes->TexturaDaMoldura);
    PacoteExiste(TEXT("o emblema do titulo"), Ajustes->EmblemaDoTitulo);

    // O escurecimento sobre a arte tem de ficar na faixa legivel: sem isso a UI some sobre a arte clara.
    TestTrue(FString::Printf(TEXT("escurecimento do fundo em faixa util (%.2f)"), Ajustes->EscurecimentoDoFundo),
        Ajustes->EscurecimentoDoFundo >= 0.f && Ajustes->EscurecimentoDoFundo <= 0.9f);
    return true;
}

#endif

