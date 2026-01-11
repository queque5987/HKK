#include "Widget/Widget_Item_Equipment.h"
#include "Widget/Object/ItemDataObject.h"
#include "Blueprint/DragDropOperation.h"
#include "GameFramework/WidgetLibrary.h"

void UWidget_Item_Equipment::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	UItemDataObject* DataObject = Cast<UItemDataObject>(InListItemObject);
	if (DataObject == nullptr) return;

	Super::NativeOnListItemObjectSet(InListItemObject);

	if (ItemIcon == nullptr) return;
	if (DataObject->ItemConfig.ItemIcon == nullptr)
	{
		ItemIcon->SetBrushFromTexture(DefaultImage);
		ItemIcon->SetRenderOpacity(0.5f);
	}
	else
	{
		ItemIcon->SetRenderOpacity(1.f);
	}
}

bool UWidget_Item_Equipment::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (InOperation == nullptr) return false;
	UItemDataObject* DraggedItemData = Cast<UItemDataObject>(InOperation->Payload);
	if (DraggedItemData == nullptr) return false;

	UItemDataObject* CurrentItemData = Cast<UItemDataObject>(ListItemObject);
	if (CurrentItemData == nullptr) return false;

	EEquipmentSlotType tempEquipmentSlot = CurrentItemData->ItemConfig.EquipmentSlotType;
	DraggedItemData->ItemConfig.EquipmentSlotType = tempEquipmentSlot;
	UWidgetLibrary::EquipmentSlotChanged(DraggedItemData->OwningPlayer, DraggedItemData, tempEquipmentSlot);
	return true;
}
