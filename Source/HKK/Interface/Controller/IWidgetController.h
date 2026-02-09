#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_Delegates.h"
#include "HKK_PCH.h"
#include "IWidgetController.generated.h"

UINTERFACE(MinimalAPI)
class UIWidgetController : public UInterface
{
	GENERATED_BODY()
};

class UUserWidget;

class HKK_API IIWidgetController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UUserWidget* CreateSimpleWidget(TSubclassOf<UUserWidget> InClass);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ItemInteractPickUpWidget(bool IsOn, UObject* PickableItemObject, const FItemConfig& ItemConfig);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeQuickSlot(UObject* ChangedItemObject, FKey ChangedKey);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ChangeEquipSlot(UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EquipmentItemDragDetected(bool e);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EEquipmentSlotType GetLeftEquipmentSlotIndex();
	virtual FOnSetItemInteractWidget& GetOnSetItemInteractPickupWidget() = 0;
	virtual FOnKeyInputEvent& GetOnKeyTriggered() = 0;
	virtual FOnKeyInputEvent& GetOnKeyReleased() = 0;
	virtual FOnGetItem& GetOnGetItem() = 0;
	virtual FOnQuickSlotUpdated& GetOnQuickSlotUpdated() = 0;
	virtual FOnItemEquiped& GetOnItemEquiped() = 0;
	virtual FOnCreateInteractWidget& GetOnCreateInteractWidget() = 0;
	virtual FVector GetPlayerLocation() = 0;
	virtual class UObject* GetPlayerStateObject() = 0;
	virtual void SetCurorVisibility(bool e) = 0;
};
