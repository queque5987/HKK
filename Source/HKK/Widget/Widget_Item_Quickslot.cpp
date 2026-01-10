#include "Widget/Widget_Item_Quickslot.h"
#include "Widget/Object/ItemDataObject.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "GameFramework/WidgetLibrary.h"
#include "Widget_Item_Quickslot.h"
#include "GameFramework/WidgetLibrary.h"
#include "HKK_Structs.h"

bool UWidget_Item_Quickslot::Initialize()
{
	bool rtn = Super::Initialize();



	return rtn;
}

void UWidget_Item_Quickslot::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	UItemDataObject* DataObject = Cast<UItemDataObject>(InListItemObject);
	if (DataObject == nullptr) return;
	if (DataObject->GetIsQuickSlotInitializer())
	{
		QuickSlotKeyType = DataObject->GetQuickSlotKey();
		if (QuickSlotIdx)
		{
			QuickSlotIdx->SetText(
				QuickSlotKeyType.GetFName() != "None" ?
				FText::FromString(QuickSlotKeyType.GetFName().ToString()) :
				FText::FromString(FString(""))
			);
		}
	}
	else
	{
		if (QuickSlotIdx)
		{
			QuickSlotIdx->SetText(
				DataObject->ItemConfig.QuickSlotKey.GetFName() != "None" ?
				FText::FromString(DataObject->ItemConfig.QuickSlotKey.GetFName().ToString()) :
				FText::FromString(FString(""))
			);
		}
	}

	Super::NativeOnListItemObjectSet(InListItemObject);
}

void UWidget_Item_Quickslot::NativeOnEntryReleased()
{
	Super::NativeOnEntryReleased();
}

bool UWidget_Item_Quickslot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (InOperation == nullptr) return false;
	UItemDataObject* DraggedItemData = Cast<UItemDataObject>(InOperation->Payload);
	if (DraggedItemData == nullptr) return false;

	UItemDataObject* CurrentItemData = Cast<UItemDataObject>(ListItemObject);
	if (CurrentItemData == nullptr) return false;

	FKey tempKey = CurrentItemData->ItemConfig.QuickSlotKey;
	//FKey tempKey = FKey(FName(QuickSlotIdx->GetText().ToString()));
	DraggedItemData->ItemConfig.QuickSlotKey = tempKey;
	UWidgetLibrary::QuickSlotChanged(DraggedItemData->OwningPlayer, DraggedItemData, DraggedItemData->ItemConfig.QuickSlotKey);
	return true;
}
