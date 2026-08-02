#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAI_MEGA_MAN_TESTModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
