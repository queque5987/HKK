#include "Widget/Widget_Inventory.h"
#include "Widget/Object/ItemDataObject.h"
#include "HKK_PCH.h"

void UWidget_Inventory::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWidget_Inventory::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	Execute_OnSetVisibility(this, InVisibility == ESlateVisibility::Collapsed || InVisibility == ESlateVisibility::Hidden ? false : true);
}

void UWidget_Inventory::OnUpdatePlayerStatFloat_Implementation(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("UI Function Called! HP: %f / %f"), CurrStat, MaxStat));
	switch (PlayerStatType)
	{
	case(EPlayerStatType::EPST_HP):
		if (HPBar == nullptr) return;
		HPBar->SetPercent(CurrStat / MaxStat);
		break;
	case(EPlayerStatType::EPST_Stamina):
		if (StaminaBar == nullptr) return;
		StaminaBar->SetPercent(CurrStat / MaxStat);
		break;
	default:
		break;
	}

}

void UWidget_Inventory::AddItem_Implementation(bool bPutIn, EUserWidget AddWidgetType, const FItemConfig& ItemConfig)
{
	if (ItemConfig.Stackable)
	{
		for (auto& ListItem : InventoryTree->GetListItems())
		{
			UItemDataObject* ItemDataObject = Cast<UItemDataObject>(ListItem);
			if (ItemDataObject == nullptr) continue;
			if (ItemConfig.ItemType == ItemDataObject->ItemConfig.ItemType)
			{
				ItemDataObject->ItemConfig.ItemCount++;
				ItemDataObject->OnItemSlotUpdated.Broadcast(ListItem);
				return;
			}
		}
	}

	UItemDataObject* DataObject = NewObject<UItemDataObject>(this);
	if (DataObject == nullptr) return;
	if (GetOwningPlayer() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_Inventory :: Owning Player Nullptr"));
	}
	DataObject->OwningPlayer = GetOwningPlayer();
	DataObject->ItemConfig = ItemConfig;
	InventoryTree->AddItem(DataObject);
}

void UWidget_Inventory::AddNewItemObject_Implementation(UObject* AddItemObject)
{
	UItemDataObject* ItemDataObject = Cast<UItemDataObject>(AddItemObject);
	ItemDataObject->OwningPlayer = GetOwningPlayer();
	InventoryTree->AddItem(ItemDataObject);
}

void UWidget_Inventory::OnUpdateQuickSlot_Implementation(UObject* UpdatedItem, FKey Key)
{
	//int8 idx = 0;
	//TArray<UObject*> NewArray;
	//for (UObject* Item : QuickSlotTile->GetListItems())
	//{
	//	UItemDataObject* ItemDataObject = Cast<UItemDataObject>(Item);
	//	FKey QuickSlotSettingKey = (*QuickSlotKeySetting)[idx];
	//	if (ItemDataObject != nullptr)
	//	{
	//		if (Item == UpdatedItem && QuickSlotSettingKey != Key)
	//		{
	//			NewArray.Add(Cast<UObject>((*QuickSlotEmptyObjects)[idx]));
	//		}
	//		else if (QuickSlotSettingKey == Key)
	//		{
	//			NewArray.Add(UpdatedItem);
	//		}
	//		else NewArray.Add(Item);
	//	}
	//	else
	//	{
	//		NewArray.Add(Cast<UObject>((*QuickSlotEmptyObjects)[idx]));
	//	}
	//	idx++;
	//}
	//QuickSlotTile->SetListItems(NewArray);
	//UItemDataObject* UpdatedItemDataObject = Cast<UItemDataObject>(UpdatedItem);
	//if (UpdatedItemDataObject != nullptr) UpdatedItemDataObject->OnItemSlotUpdated.Broadcast(UpdatedItem);
}

void UWidget_Inventory::UpdateQuickSlotObjectArr(TArray<UObject*>& InQuickSlotObjectArr)
{
	if (QuickSlotTile) QuickSlotTile->SetListItems(InQuickSlotObjectArr);
}

void UWidget_Inventory::UpdateEquipmentSlotObjectArr(TArray<UObject*>& InEquipmentSlotObjectArr)
{
	if (EquipmentSlotTile) EquipmentSlotTile->SetListItems(InEquipmentSlotObjectArr);
}

const TArray<UObject*>& UWidget_Inventory::GetQuickSlotObjectArr()
{
	return QuickSlotTile->GetListItems();
}

const TArray<UObject*>& UWidget_Inventory::GetEquipmentSlotObjectArr()
{
	return EquipmentSlotTile->GetListItems();
}

void UWidget_Inventory::QuickSlot_AddItemAsObject(UObject* InItem)
{
	if (InItem != nullptr) QuickSlotTile->AddItem(InItem);
}

void UWidget_Inventory::EquipmentSlot_AddItemAsObject(UObject* InItem)
{
	if (InItem != nullptr) EquipmentSlotTile->AddItem(InItem);
}
