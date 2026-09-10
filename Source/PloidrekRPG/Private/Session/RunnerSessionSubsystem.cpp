#include "Session/RunnerSessionSubsystem.h"

#include "Session/RunnerSession.h"

void URunnerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    Session = NewObject<URunnerSession>(this, TEXT("RunnerSession"));
}
