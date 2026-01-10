#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_Delegates.h"
#include "IWidgetController.generated.h"

UINTERFACE(MinimalAPI)
class UIWidgetController : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IIWidgetController
{
	GENERATED_BODY()

public:
	virtual FOnSetItemInteractWidget& GetOnSetItemInteractPickupWidget() = 0;
	virtual FOnKeyInputEvent& GetOnKeyTriggered() = 0;
	virtual FOnKeyInputEvent& GetOnKeyReleased() = 0;
	virtual FOnGetItem& GetOnGetItem() = 0;
	virtual FOnQuickSlotUpdated& GetOnQuickSlotUpdated() = 0;
	virtual FVector GetPlayerLocation() = 0;
	virtual class UObject* GetPlayerStateObject() = 0;
	virtual void SetCurorVisibility(bool e) = 0;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeQuickSlot(UObject* ChangedItemObject, FKey ChangedKey);
};
