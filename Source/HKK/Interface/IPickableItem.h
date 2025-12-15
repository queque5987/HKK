#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_PCH.h"
#include "IPickableItem.generated.h"

UINTERFACE(MinimalAPI)
class UIPickableItem : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IIPickableItem
{
	GENERATED_BODY()

public:

	virtual void OnItemStencilValueChange(ECustomStencilValue CustomStencilValue) {};
	virtual FComponentBeginOverlapSignature* GetComponentBeginOverlapSignature() { return nullptr; };
	virtual FComponentEndOverlapSignature* GetComponentEndOverlapSignature() { return nullptr; };
};
