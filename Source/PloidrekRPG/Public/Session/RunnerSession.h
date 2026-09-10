#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Data/RunnerTypes.h"
#include "Data/RunnerCharacterProfile.h"
#include "Data/RunnerChassisData.h"
#include "Data/RunnerClassData.h"
#include "RunnerSession.generated.h"

class UDataTable;
class FUniqueNetId;

/** Uma conta local: o e-mail e a identidade (chave) e o nome de usuario e o que aparece na tela. */
USTRUCT()
struct PLOIDREKRPG_API FRunnerLocalAccount
{
    GENERATED_BODY()

    UPROPERTY() FString Email;
    UPROPERTY() FString UserName;
    UPROPERTY() FString PasswordHash;
};

/** Um personagem salvo na conta — e o que aparece na tela de selecao depois do login. */
USTRUCT(BlueprintType)
struct PLOIDREKRPG_API FRunnerSavedCharacter
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FString AccountName;
    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FString CharacterId;
    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FName ChassisId;
    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FName ClassId;

    /** O nome que o jogador deu ao personagem na criacao ("Volt", "Kardys-7"). */
    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FString CharacterName;

    /** Como aparece na lista: o nome do personagem, ou chassi + classe enquanto ele nao tem nome. */
    FString GetDisplayName() const
    {
        if (!CharacterName.IsEmpty())
        {
            return CharacterName;
        }
        return FString::Printf(TEXT("%s · %s"), *ChassisId.ToString(), *ClassId.ToString());
    }
};

/** Aviso de fim de login. O caminho EOS e assincrono; o local responde na hora. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRunnerLoginComplete, bool, bSuccess, const FString&, Error);

/**
 * A maquina de estado da sessao: conta -> personagens da conta -> chassi -> classe -> ficha.
 * E um UObject simples de proposito: nenhuma dependencia de mundo ou de GameInstance,
 * entao o fluxo inteiro pode ser exercitado por teste de automacao.
 *
 * Login local (dev/offline): Saved/RunnerAccounts.tsv (senha com hash MD5).
 * Login de plataforma: Epic Online Services pela Auth Interface (IOnlineIdentity).
 * Personagens: Saved/RunnerCharacters.tsv — uma linha por personagem da conta.
 */
UCLASS()
class PLOIDREKRPG_API URunnerSession : public UObject
{
    GENERATED_BODY()

public:
    /** Disparado quando o login termina (sucesso ou falha com motivo). */
    UPROPERTY(BlueprintAssignable, Category = "Runner|Sessao")
    FRunnerLoginComplete OnLoginComplete;

    // ---------- Conta local (dev / offline) ----------
    /**
     * Regras de uma conta nova, sem tocar em estado: a tela mostra o erro e o teste cobra a regra.
     * E-mail: um "@", sem espacos e com dominio de ponto (nome@dominio.com).
     * Nome de usuario: 3+ caracteres, so letras, numeros, _ ou -.
     * Senha: 8+ caracteres com maiuscula, minuscula e caractere especial; confirmacao igual a senha.
     * OutError recebe a PRIMEIRA regra que falhou, ja no texto que o jogador le.
     */
    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    static bool ValidateNewAccount(const FString& Email, const FString& UserName,
                                   const FString& Password, const FString& ConfirmPassword,
                                   FString& OutError);

    /** Cria a conta local: o e-mail e a chave, o nome de usuario e o que aparece na tela. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    bool CreateAccount(const FString& Email, const FString& UserName, const FString& Password, FString& OutError);

    /** Entra na conta local pelo e-mail e senha. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    bool Login(const FString& Email, const FString& Password, FString& OutError);

    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    void Logout();

    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    bool IsLoggedIn() const { return bLoggedIn; }

    /** Nome que aparece na tela: nome de usuario no login local, nickname no EOS. */
    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    FString GetAccountName() const { return AccountName; }

    /** E-mail da conta local (vazio quando o login foi pelo EOS). */
    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    FString GetAccountEmail() const { return AccountEmail; }

    // ---------- Login de plataforma (Epic Online Services) ----------
    /** O subsistema EOS existe neste build? (depende dos plugins e do Config) */
    UFUNCTION(BlueprintPure, Category = "Runner|EOS")
    bool IsEOSAvailable() const;

    /**
     * Login pela Epic Online Services (Auth Interface).
     * AuthType "developer": usa Id/Token do Dev Auth Tool da Epic.
     * AuthType "accountportal": abre o portal de conta da Epic no navegador.
     * AuthType "persistentauth": reusa a sessao anterior.
     * O resultado chega por OnLoginComplete.
     */
    UFUNCTION(BlueprintCallable, Category = "Runner|EOS")
    bool LoginWithEOS(const FString& AuthType, const FString& Id, const FString& Token, FString& OutError);

    UFUNCTION(BlueprintCallable, Category = "Runner|EOS")
    void LogoutEOS();

    // ---------- Personagens da conta ----------
    /** Lista os personagens da conta logada (vazio se nenhum). */
    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    TArray<FRunnerSavedCharacter> GetSavedCharacters() const;

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    bool HasSavedCharacters() const;

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    int32 GetMaxCharacterSlots() const { return 8; }

    /** Entra no jogo com um personagem ja existente da conta. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool SelectSavedCharacter(const FString& CharacterId, FString& OutError);

    // ---------- Selecao e ficha ----------
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool SelectChassis(FName ChassisId, FString& OutError);

    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool SelectClass(FName ClassId, FString& OutError);

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    FName GetSelectedChassis() const { return SelectedChassis; }

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    FName GetSelectedClass() const { return SelectedClass; }

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    bool IsSelectionComplete() const;

    /** Cria um personagem novo, salva na conta e deixa como ficha ativa. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool CreateCharacterProfile(FRunnerCharacterProfile& OutProfile, FString& OutError);

    // ---------- Nome do personagem ----------
    /**
     * Regra do nome do personagem (na tela de criacao): 3 a 16 caracteres, letras, numeros,
     * espaco, _ ou -. Pura de proposito: a tela mostra o erro e o teste cobra a regra.
     */
    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    static bool ValidateCharacterName(const FString& Name, FString& OutError);

    /** Nome escolhido na tela de criacao, usado pelo CreateCharacterProfile. */
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    void SetPendingCharacterName(const FString& Name) { PendingCharacterName = Name.TrimStartAndEnd(); }

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    FString GetPendingCharacterName() const { return PendingCharacterName; }

    /** Esse nome ja e de outro personagem desta conta? */
    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    bool IsCharacterNameTaken(const FString& Name) const;

    /** Seleciona o primeiro personagem salvo da conta (compatibilidade). */
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool RestoreCharacter(FString& OutError);

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    bool HasCharacter() const { return bHasCharacter; }

    UFUNCTION(BlueprintPure, Category = "Runner|Criacao")
    bool GetActiveProfile(FRunnerCharacterProfile& OutProfile) const;

    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    void ClearCharacter();

    // ---------- Dados ----------
    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    UDataTable* GetChassisTable() const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    UDataTable* GetClassTable() const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    TArray<FName> GetChassisIds(bool bIncludeExtinct) const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    TArray<FName> GetClassIds() const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    bool GetChassisData(FName ChassisId, FRunnerChassisData& OutChassis) const;

    UFUNCTION(BlueprintPure, Category = "Runner|Dados")
    bool GetClassData(FName ClassId, FRunnerClassData& OutClass) const;

private:
    void LoadAccountsIfNeeded();
    bool SaveAccounts() const;
    bool SaveCharacters() const;
    static FString HashPassword(const FString& Password);

    const FRunnerSavedCharacter* FindSavedCharacter(const FString& CharacterId) const;
    /** Key = identidade da conta (e-mail no local, conta EOS na plataforma); DisplayName = o que aparece. */
    void CompleteLogin(const FString& Key, const FString& DisplayName);

    /** Callback do login assincrono do EOS. */
    void HandleEOSLoginComplete(int32 LocalUserNumber, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

    UPROPERTY() TMap<FString, FRunnerLocalAccount> Accounts;
    UPROPERTY() TArray<FRunnerSavedCharacter> SavedCharacters;

    FRunnerCharacterProfile ActiveProfile;
    bool bHasCharacter = false;
    bool bAccountsLoaded = false;
    bool bLoggedIn = false;
    FString AccountName;
    FString AccountEmail;
    /** Identidade da conta: e-mail no local, conta EOS na plataforma. E a chave dos personagens salvos. */
    FString AccountKey;
    FName SelectedChassis;
    FName SelectedClass;
    FString PendingCharacterName;
};
