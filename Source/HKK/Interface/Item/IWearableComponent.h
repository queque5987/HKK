#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IWearableComponent.generated.h"

UINTERFACE(MinimalAPI)
class UIWearableComponent : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IIWearableComponent
{
	GENERATED_BODY()

public:

	virtual class USceneComponent* GetAttachComponent() = 0;
	
};
