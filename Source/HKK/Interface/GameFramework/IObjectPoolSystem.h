#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IObjectPoolSystem.generated.h"

UINTERFACE(MinimalAPI)
class UIObjectPoolSystem : public UInterface
{
	GENERATED_BODY()
};

class AActor;

class HKK_API IIObjectPoolSystem
{
	GENERATED_BODY()

public:
	
	virtual AActor* GetFromPool(UClass* InClass) { return nullptr; };
	virtual bool ReturnToPool(UClass* InClass, AActor* InActor) { return bool(); };
};
