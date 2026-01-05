#include "Widget/Widget_Item_Inventory.h"
#include "Widget/Object/ItemDataObject.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/DragDropOperation.h"
#include "GameFramework/CombatLibrary.h"

void UWidget_Item_Inventory::NativeOnListItemObjectSet(UObject* InListItemObject)
{
	ListItemObject = InListItemObject;

	UItemDataObject* DataObject = Cast<UItemDataObject>(ListItemObject);
	if (DataObject == nullptr) return;

	const FItemConfig& ItemConfig = DataObject->ItemConfig;
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(ItemConfig.ItemIcon);
	}
	if (ItemName)
	{
		ItemName->SetText(ItemConfig.ItemName);
	}
	if (QuickSlotIdx)
	{
		QuickSlotIdx->SetText(
			ItemConfig.QuickSlotKey.GetFName() != "None" ?
			FText::FromString(ItemConfig.QuickSlotKey.GetFName().ToString()) :
			FText::FromString(FString(""))
		);
	}
	if (ItemCount)
	{
		if (ItemConfig.ItemCount > 0)
		{
			ItemCount->SetText(FText::FromString(FString::FromInt(ItemConfig.ItemCount)));

			ItemCount->SetVisibility(ESlateVisibility::Visible);
			ItemCountX->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ItemCount->SetVisibility(ESlateVisibility::Collapsed);
			ItemCountX->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (DataObject->OwningPlayer != nullptr)
	{
		SetOwningPlayer(DataObject->OwningPlayer);
	}

	if (OnItemSlotUpdated_DelegatePtr != nullptr && OnItemSlotUpdatedDelegateHandle.IsValid())
	{
		OnItemSlotUpdated_DelegatePtr->Remove(OnItemSlotUpdatedDelegateHandle);
		OnItemSlotUpdatedDelegateHandle.Reset();
		OnItemSlotUpdated_DelegatePtr = nullptr;
	}
	OnItemSlotUpdatedDelegateHandle = DataObject->OnItemSlotUpdated.AddUFunction(this, TEXT("NativeOnListItemObjectSet"));
	OnItemSlotUpdated_DelegatePtr = &DataObject->OnItemSlotUpdated;
}

void UWidget_Item_Inventory::NativeOnEntryReleased()
{
	if (OnItemSlotUpdated_DelegatePtr != nullptr && OnItemSlotUpdatedDelegateHandle.IsValid())
	{
		OnItemSlotUpdated_DelegatePtr->Remove(OnItemSlotUpdatedDelegateHandle);
		OnItemSlotUpdatedDelegateHandle.Reset();
		OnItemSlotUpdated_DelegatePtr = nullptr;
	}
}

FReply UWidget_Item_Inventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	// Check for left mouse button press to initiate drag
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// DetectDragIfPressed returns a reply that will start OnDragDetected if the mouse moves far enough.
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
	}

	return FReply::Unhandled();
}

void UWidget_Item_Inventory::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	// Don't drag if our item data is invalid
	if (ListItemObject == nullptr)
	{
		return;
	}

	// Create the drag visual widget
	UWidget_Item_Inventory* DragVisual = CreateWidget<UWidget_Item_Inventory>(GetOwningPlayer(), DragVisualClass);

	if (GetOwningPlayer() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning Player Nullptr"));
	}
	if (DragVisual)
	{
		// Pass the item data to the new visual widget. No need to check for interface, as the class type is guaranteed.
		DragVisual->NativeOnListItemObjectSet(ListItemObject);

		// Create a new Drag and Drop Operation
		UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));
		if (DragOperation)
		{
			// The Payload is the data we want to transfer
			DragOperation->Payload = ListItemObject;
			// The DefaultDragVisual is the widget that will follow the mouse
			DragOperation->DefaultDragVisual = DragVisual;

			// OutOperation is an output parameter that the engine will use.
			OutOperation = DragOperation;
		}
	}
}

bool UWidget_Item_Inventory::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// This function is called every frame a drag operation is over this widget.
	// We can add visual feedback here (e.g., change color).
	return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
}

bool UWidget_Item_Inventory::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	if (InOperation == nullptr) return false;
	UItemDataObject* DraggedItemData = Cast<UItemDataObject>(InOperation->Payload);
	if (DraggedItemData == nullptr) return false;

	UItemDataObject* CurrentItemData = Cast<UItemDataObject>(ListItemObject);
	if (CurrentItemData == nullptr) return false;

	FKey tempKey = CurrentItemData->ItemConfig.QuickSlotKey;
	//CurrentItemData->ItemConfig = DraggedItemData->ItemConfig;
	//CurrentItemData->ItemConfig.QuickSlotKey = tempKey;
	DraggedItemData->ItemConfig.QuickSlotKey = tempKey;
	CurrentItemData->OnItemSlotUpdated.Broadcast(InOperation->Payload);
	//NativeOnListItemObjectSet(CurrentItemData);

	//UObject* OwningPlayerControllerObject = GetOwningPlayer();
	//UCombatLibrary::RefreshQuickSlot(OwningPlayerControllerObject);
	return true;
}

void UWidget_Item_Inventory::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// Called when a drag operation first enters this widget's bounds.
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UWidget_Item_Inventory::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// Called when a drag operation leaves this widget's bounds.
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

void UWidget_Item_Inventory::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// Called if the drag is cancelled (e.g., by pressing Esc).
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}
