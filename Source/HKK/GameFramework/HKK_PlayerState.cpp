#include "GameFramework/HKK_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Interface/Widget/IUpdatableWidget.h"
#include "Widget/Object/ItemDataObject.h"

AHKK_PlayerState::AHKK_PlayerState() : Super()
{
	QuickSlotItemData.SetNum(3);
	EquipmentSlotItemConfig.SetNum(4);
	CurrentEquipSlotIndex = 0;
}

void AHKK_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHKK_PlayerState, CurrHP);
	DOREPLIFETIME(AHKK_PlayerState, MaxHP);
	DOREPLIFETIME(AHKK_PlayerState, CurrStamina);
	DOREPLIFETIME(AHKK_PlayerState, MaxStamina);
	//DOREPLIFETIME(AHKK_PlayerState, PossesingItem);
	//DOREPLIFETIME(AHKK_PlayerState, PossingItemData);
	DOREPLIFETIME(AHKK_PlayerState, PosseingItemConfig);
	DOREPLIFETIME(AHKK_PlayerState, RecentlyAddedItemConfig);
	DOREPLIFETIME(AHKK_PlayerState, CurrentEquipSlotIndex);
	DOREPLIFETIME(AHKK_PlayerState, EquipmentSlotItemConfig);
}

bool AHKK_PlayerState::BindDelegate_HUDWidget_Implementation(UObject* BindWidget)
{
	if (BindWidget == nullptr) return false;
	OnUpdateStatFloat.AddUFunction(BindWidget, "OnUpdatePlayerStatFloat");
	OnEquipmentSlotSwitched.AddUFunction(BindWidget, "EquipItem");
	IIUpdatableWidget::Execute_OnUpdatePlayerStatFloat(BindWidget, EPlayerStatType::EPST_HP, 1.f, 1.f);
	IIUpdatableWidget::Execute_OnUpdatePlayerStatFloat(BindWidget, EPlayerStatType::EPST_Stamina, 1.f, 1.f);
	return OnUpdateStatFloat.IsBoundToObject(BindWidget);
}

bool AHKK_PlayerState::BindDelegate_InventoryWidget_Implementation(UObject* BindWidget)
{
	if (BindWidget == nullptr) return false;
	//OnUpdateItemToInventory.AddUFunction(BindWidget, "AddItem"); // Deprecated
	OnAddItemDataObject.AddUFunction(BindWidget, "AddNewItemObject");
	OnEquipmentSlotSwitched.AddUFunction(BindWidget, "EquipItem");
	UE_LOG(LogTemp, Warning, TEXT("[Client %d] Delegate Bound on Widget"),
		GetPlayerId());
	return OnAddItemDataObject.IsBoundToObject(BindWidget);
}

bool AHKK_PlayerState::BindDelegate_Equipment_Implementation(UObject* BindCharacter)
{
	OnEquipmentSlotSwitched.AddUFunction(BindCharacter, "OnEquipmentSlotSwitched");
	return OnEquipmentSlotSwitched.IsBoundToObject(BindCharacter);
}

void AHKK_PlayerState::GetItem_Implementation(const FItemConfig& ItemConfig)
{
	UE_LOG(LogTemp, Log, TEXT("Get Item : %s"), *ItemConfig.ItemName.ToString());
	Server_GetItem(ItemConfig);
	return;
}

void AHKK_PlayerState::GetEquipmentSlotItemConfig_Implementation(TArray<FItemConfig>& InEquipmentSlotItemConfig)
{
	InEquipmentSlotItemConfig = EquipmentSlotItemConfig;
}

void AHKK_PlayerState::Server_SetCurrHP_Implementation(float e)
{
	CurrHP = e;
	if (HasAuthority()) OnRep_CurrHP();
}

void AHKK_PlayerState::Server_SetCurrStamina_Implementation(float e)
{
	CurrStamina = e;
	if (HasAuthority()) OnRep_CurrStamina();
}

void AHKK_PlayerState::Server_SetMaxHP_Implementation(float e)
{
	MaxHP = e;
	if (HasAuthority()) OnRep_MaxHP();
}

void AHKK_PlayerState::Server_SetMaxStamina_Implementation(float e)
{
	MaxStamina = e;
	if (HasAuthority()) OnRep_MaxStamina();
}

void AHKK_PlayerState::Server_GetItem_Implementation(const FItemConfig& ItemConfig)
{
	UE_LOG(LogTemp, Log, TEXT("Server Get Item : %s"), *ItemConfig.ItemName.ToString());

	bool IsStackable = ItemConfig.Stackable;
	bool IsAdded = false;

	if (IsStackable)
	{
		for (FItemConfig& IterConfig : PosseingItemConfig)
		{
			if (ItemConfig.ItemType == IterConfig.ItemType)
			{
				IterConfig.ItemCount += ItemConfig.ItemCount;
				IsAdded = true;
				RecentlyAddedItemConfig = IterConfig;
				break;
			}
		}
	}
	if (!IsAdded)
	{
		PosseingItemConfig.Add(ItemConfig);
		RecentlyAddedItemConfig = ItemConfig;
		if (HasAuthority()) OnRep_RecentlyAddedItemConfig();
	}

	//PossesingItem.Add(ItemConfig);
	//if (HasAuthority()) OnRep_PossesingItem();
}

//void AHKK_PlayerState::Server_EquipmentSlotChanged_Implementation(UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType)
void AHKK_PlayerState::Server_EquipmentSlotChanged_Implementation(FItemConfig ChangedItemConfig, EEquipmentSlotType EquipmentSlotType)
{
	uint8 QuickslotIndex = StaticCast<uint8>(EquipmentSlotType);
	//UItemDataObject* ChangedDataObject = Cast<UItemDataObject>(ChangedItemObject);
	//EquipmentSlotItemData[QuickslotIndex] = ChangedDataObject;
	EquipmentSlotItemConfig[QuickslotIndex] = ChangedItemConfig;
	uint8 idx = 0;
	for (FItemConfig& SlotItemConfig : EquipmentSlotItemConfig)
	{
		idx++;
		EEquipmentSlotType CurrEquipSlotType = StaticCast<EEquipmentSlotType>(idx - 1);
		if (SlotItemConfig.EquipmentSlotType != CurrEquipSlotType)
		{
			SlotItemConfig = FItemConfig();
		}
	}
	//for (TWeakObjectPtr<UItemDataObject>& SlotItemData : EquipmentSlotItemData)
	//{
	//	idx++;
	//	if (!SlotItemData.IsValid()) continue;
	//	EEquipmentSlotType CurrEquipSlotType = StaticCast<EEquipmentSlotType>(idx - 1);
	//	UItemDataObject* DataObject = SlotItemData.Get();

	//	if (DataObject == ChangedDataObject && DataObject->ItemConfig.EquipmentSlotType != CurrEquipSlotType)
	//	{
	//		SlotItemData = nullptr;
	//	}

	//}
}

void AHKK_PlayerState::Server_QuickSlotChanged_Implementation(UObject* ChangedItemObject, FKey ChangedKey)
{
	UItemDataObject* ChangedDataObject = Cast<UItemDataObject>(ChangedItemObject);
	for (int8 i = 1; i < 4; i++)
	{
		FKey CurrentQuickSlotKey = FKey(*FString::FromInt(i));
		if (CurrentQuickSlotKey == ChangedKey)
		{
			QuickSlotItemData[i - 1] = ChangedDataObject;
		}
		else if (QuickSlotItemData[i - 1].Get() == ChangedDataObject)
		{
			QuickSlotItemData[i - 1] = nullptr;
		}
	}
}

void AHKK_PlayerState::OnRep_RecentlyAddedItemConfig()
{
	if (RecentlyAddedItemConfig.Stackable)
	{
		for (UItemDataObject* ItemDataObject : PossingItemData)
		{
			if (ItemDataObject == nullptr) continue;
			if (RecentlyAddedItemConfig.ItemType == ItemDataObject->ItemConfig.ItemType)
			{
				ItemDataObject->ItemConfig.ItemCount = RecentlyAddedItemConfig.ItemCount;
				ItemDataObject->OnItemSlotUpdated.Broadcast(Cast<UObject>(ItemDataObject));
				return;
			}
		}
	}
	
	UItemDataObject* DataObject = NewObject<UItemDataObject>(this);
	DataObject->OwningPlayer = GetPlayerController();
	DataObject->ItemConfig = RecentlyAddedItemConfig;
	DataObject->OnItemSlotUpdated.AddUFunction(this, TEXT("OnItemDataUpdated"));
	PossingItemData.Add(DataObject);
	OnAddItemDataObject.Broadcast(DataObject);
	UE_LOG(LogTemp, Warning, TEXT("[Client %d] Delegate Fired on Widget"), GetPlayerId());
	

	//if (RecentlyAddedItemObject != nullptr)
	//{
	//	OnAddItemDataObject.Broadcast(RecentlyAddedItemObject);
	//	RecentlyAddedItemObject = nullptr;
	//}
}

void AHKK_PlayerState::Server_SetCuurentEquipSlotIndex_Implementation(int32 NewSlotIndex)
{
	CurrentEquipSlotIndex = NewSlotIndex;
	if (HasAuthority()) OnRep_CurrentEquipSlotIndex();
}

void AHKK_PlayerState::OnRep_CurrStamina()
{
	OnUpdateStatFloat.Broadcast(EPlayerStatType::EPST_Stamina, CurrStamina, MaxStamina);
}

void AHKK_PlayerState::OnRep_CurrHP()
{
	OnUpdateStatFloat.Broadcast(EPlayerStatType::EPST_HP, CurrHP, MaxHP);
	//if (GetOwningController() != nullptr && OnUpdateStatFloat.IsBound()) DrawDebugString(GetWorld(), FVector(0.f, 0.f, 120.f), TEXT("Hit"), GetOwningController()->GetPawn(), FColor::Red, 2.f);
}

void AHKK_PlayerState::OnRep_MaxStamina()
{
	OnUpdateStatFloat.Broadcast(EPlayerStatType::EPST_Stamina, CurrStamina, MaxStamina);
}

void AHKK_PlayerState::OnRep_MaxHP()
{
	OnUpdateStatFloat.Broadcast(EPlayerStatType::EPST_HP, CurrHP, MaxHP);
}

void AHKK_PlayerState::OnRep_CurrentEquipSlotIndex()
{
	if (CurrentEquipSlotIndex == 0)
	{
		// BareHand
		OnEquipmentSlotSwitched.Broadcast(FItemConfig());
	}
	else if (EquipmentSlotItemConfig[CurrentEquipSlotIndex].WearableSpawnClass != nullptr)
	{
		OnEquipmentSlotSwitched.Broadcast(EquipmentSlotItemConfig[CurrentEquipSlotIndex]);
	}
}

void AHKK_PlayerState::OnItemDataUpdated(UObject* UpdatedItem)
{
	UItemDataObject* ItemDataObject = Cast<UItemDataObject>(UpdatedItem);
	if (ItemDataObject == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AHKK_PlayerState Updated Item Nullptr"));
		return;
	}
	FKey QuickSlotKey = ItemDataObject->ItemConfig.QuickSlotKey;
	if (QuickSlotKey == FKey("1"))
	{
		if (!QuickSlotItemData.IsValidIndex(0))
		{
			UE_LOG(LogTemp, Warning, TEXT("AHKK_PlayerState Slot Index 0 Is Not Valid Index"));
			return;
		}
		QuickSlotItemData[0] = ItemDataObject;
	}
	else if (QuickSlotKey == FKey("2"))
	{
		if (!QuickSlotItemData.IsValidIndex(10))
		{
			UE_LOG(LogTemp, Warning, TEXT("AHKK_PlayerState Slot Index 1 Is Not Valid Index"));
			return;
		}
		QuickSlotItemData[1] = ItemDataObject;
	}
	else if (QuickSlotKey == FKey("3"))
	{
		if (!QuickSlotItemData.IsValidIndex(2))
		{
			UE_LOG(LogTemp, Warning, TEXT("AHKK_PlayerState Slot Index 2 Is Not Valid Index"));
			return;
		}
		QuickSlotItemData[2] = ItemDataObject;
	}
	int8 idx = 0;
	for (TWeakObjectPtr<UItemDataObject> WeakDataObject : QuickSlotItemData)
	{
		if (WeakDataObject.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("QuickSlot %d %s"), idx, *WeakDataObject.Get()->ItemConfig.ItemName.ToString());
		}
		idx++;
	}
}

void AHKK_PlayerState::Callback_OnEquipmentItemSlotChanged(UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType)
{
	UItemDataObject* ItemDataObject = Cast<UItemDataObject>(ChangedItemObject);
	if (ItemDataObject == nullptr) return;
	Server_EquipmentSlotChanged(ItemDataObject->ItemConfig, EquipmentSlotType);
}

void AHKK_PlayerState::Callback_ChangedQuickSlot(UObject* ChangedItemObject, FKey ChangedKey)
{
	//Server_QuickSlotChanged(ChangedItemObject, ChangedKey);
	// TODO : Change To FItemConfig
}

void AHKK_PlayerState::Callback_KeyTriggered(FKey Key)
{
	if (Key == FKey("MouseScrollUp") || Key == FKey("MouseScrollDown"))
	{
		bool Flag = false;
		int8 SearchIdx = CurrentEquipSlotIndex;
		for (int8 i = 0; i < 4; (Key == FKey("MouseScrollUp") ? i-- : i++))
		{
			SearchIdx++;
			SearchIdx %= 4;
			//if (EquipmentSlotItemData[SearchIdx].IsValid() || SearchIdx == 0)
			if (EquipmentSlotItemConfig[SearchIdx].WearableSpawnClass != nullptr || SearchIdx == 0)
			{
				Flag = true;
				break;
			}
		}
		Server_SetCuurentEquipSlotIndex(Flag ? SearchIdx : 0);
	}
}
