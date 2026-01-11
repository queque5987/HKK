#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_PCH.h"
#include "HKK_Structs.h"
#include "IUpdatableWidget.generated.h"

UINTERFACE(MinimalAPI)
class UIUpdatableWidget : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IIUpdatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnUpdatePlayerStatFloat(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddItem(bool bPutIn, EUserWidget AddWidgetType, const FItemConfig& ItemConfig);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddNewItemObject(UObject* AddItemObject);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnUpdateQuickSlot(UObject* UpdatedItem, FKey Key);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnKeyTriggered(FKey Key);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnSetVisibility(bool e);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnEquipmentItemDragDetected(bool IsOn);
};
