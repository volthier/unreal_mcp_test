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

/** Um personagem salvo na conta — e o que aparece na tela de selecao depois do login. */
USTRUCT(BlueprintType)
struct PLOIDREKRPG_API FRunnerSavedCharacter
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FString AccountName;
    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FString CharacterId;
    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FName ChassisId;
    UPROPERTY(BlueprintReadOnly, Category = "Personagem") FName ClassId;

    /** Como aparece na lista: "Vitaspark · Blaster". */
    FString GetDisplayName() const
    {
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
    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    bool CreateAccount(const FString& Account, const FString& Password, FString& OutError);

    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    bool Login(const FString& Account, const FString& Password, FString& OutError);

    UFUNCTION(BlueprintCallable, Category = "Runner|Sessao")
    void Logout();

    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    bool IsLoggedIn() const { return bLoggedIn; }

    UFUNCTION(BlueprintPure, Category = "Runner|Sessao")
    FString GetAccountName() const { return AccountName; }

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
    void CompleteLogin(const FString& Account);

    /** Callback do login assincrono do EOS. */
    void HandleEOSLoginComplete(int32 LocalUserNumber, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

    UPROPERTY() TMap<FString, FString> Accounts;
    UPROPERTY() TArray<FRunnerSavedCharacter> SavedCharacters;

    FRunnerCharacterProfile ActiveProfile;
    bool bHasCharacter = false;
    bool bAccountsLoaded = false;
    bool bLoggedIn = false;
    FString AccountName;
    FName SelectedChassis;
    FName SelectedClass;
};
