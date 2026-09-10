#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/RunnerTypes.h"
#include "Data/RunnerCharacterProfile.h"
#include "Data/RunnerChassisData.h"
#include "Data/RunnerClassData.h"
#include "RunnerSessionSubsystem.generated.h"

class UDataTable;

/**
 * O fluxo do jogo inteiro em um lugar: conta -> selecao de chassi -> selecao de classe -> ficha.
 * A UI so chama isto e le o resultado; nenhuma regra vive em widget.
 *
 * Login local (dev-grade): contas em Saved/RunnerAccounts.tsv com hash MD5 da senha.
 * A troca por EOS depois afeta so a implementacao destas funcoes, nao o fluxo.
 */
UCLASS()
class PLOIDREKRPG_API URunnerSessionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // ---------- Conta ----------
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

    // ---------- Criacao ----------
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

    /** Monta a ficha final do personagem (chassi + classe). */
    UFUNCTION(BlueprintCallable, Category = "Runner|Criacao")
    bool CreateCharacterProfile(FRunnerCharacterProfile& OutProfile, FString& OutError) const;

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
    static FString HashPassword(const FString& Password);

    UPROPERTY() TMap<FString, FString> Accounts;
    bool bAccountsLoaded = false;
    bool bLoggedIn = false;
    FString AccountName;
    FName SelectedChassis;
    FName SelectedClass;
};
