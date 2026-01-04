#include "Widget/Widget_Item_Inventory.h"
#include "Widget/Object/ItemDataObject.h"

void UWidget_Item_Inventory::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UItemDataObject* DataObject = Cast<UItemDataObject>(ListItemObject);
	FItemConfig ItemConfig;
	if (DataObject == nullptr) return;

	ItemConfig = DataObject->ItemConfig;
	ItemIcon->SetBrushFromTexture(ItemConfig.ItemIcon);
	ItemName->SetText(ItemConfig.ItemName);
}
