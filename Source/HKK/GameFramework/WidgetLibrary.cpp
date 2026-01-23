#include "GameFramework/WidgetLibrary.h"
#include "Interface/Controller/IWidgetController.h"
#include "Interface/IPickableItem.h"

bool UWidgetLibrary::ItemInteractWidget(bool IsOn, UObject* OwningPlayerController, UObject* PickableItemObject, const FItemConfig& ItemConfig)
{
	if (IIPickableItem::Execute_GetPickableItem(PickableItemObject) || !IsOn)
	{
		IIWidgetController::Execute_ItemInteractPickUpWidget(OwningPlayerController, IsOn, PickableItemObject, ItemConfig);
		return true;
	}
	return false;
}

void UWidgetLibrary::QuickSlotChanged(UObject* ChangedPlayerController, UObject* ChangedItemObject, FKey ChangedKey)
{
	IIWidgetController::Execute_ChangeQuickSlot(ChangedPlayerController, ChangedItemObject, ChangedKey);
}

void UWidgetLibrary::EquipmentSlotChanged(UObject* ChangedPlayerController, UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType)
{
	IIWidgetController::Execute_ChangeEquipSlot(ChangedPlayerController, ChangedItemObject, EquipmentSlotType);
}

void UWidgetLibrary::EquipmentItemDragDetected(UObject* OwningPlayerController, bool e)
{
	IIWidgetController::Execute_EquipmentItemDragDetected(OwningPlayerController, e);
}

EEquipmentSlotType UWidgetLibrary::GetLeftEquipmentSlotIndex(UObject* OwningPlayerController)
{
	return IIWidgetController::Execute_GetLeftEquipmentSlotIndex(OwningPlayerController);
}
