#include "GameFramework/HKK_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Interface/Widget/IUpdatableWidget.h"
#include "Widget/Object/ItemDataObject.h"

AHKK_PlayerState::AHKK_PlayerState() : Super()
{
	QuickSlotItemData.SetNum(3);
}

void AHKK_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHKK_PlayerState, CurrHP);
	DOREPLIFETIME(AHKK_PlayerState, MaxHP);
	DOREPLIFETIME(AHKK_PlayerState, CurrStamina);
	DOREPLIFETIME(AHKK_PlayerState, MaxStamina);
	//DOREPLIFETIME(AHKK_PlayerState, PossesingItem);
	DOREPLIFETIME(AHKK_PlayerState, PossingItemData);
}

bool AHKK_PlayerState::BindDelegate_HUDWidget_Implementation(UObject* BindWidget)
{
	if (BindWidget == nullptr) return false;
	OnUpdateStatFloat.AddUFunction(BindWidget, "OnUpdatePlayerStatFloat");
	IIUpdatableWidget::Execute_OnUpdatePlayerStatFloat(BindWidget, EPlayerStatType::EPST_HP, 1.f, 1.f);
	IIUpdatableWidget::Execute_OnUpdatePlayerStatFloat(BindWidget, EPlayerStatType::EPST_Stamina, 1.f, 1.f);
	return OnUpdateStatFloat.IsBoundToObject(BindWidget);
}

bool AHKK_PlayerState::BindDelegate_InventoryWidget_Implementation(UObject* BindWidget)
{
	if (BindWidget == nullptr) return false;
	OnUpdateItemToInventory.AddUFunction(BindWidget, "AddItem"); // Deprecated
	OnAddItemDataObject.AddUFunction(BindWidget, "AddNewItemObject");
	return OnUpdateItemToInventory.IsBoundToObject(BindWidget);
}

void AHKK_PlayerState::GetItem_Implementation(const FItemConfig& ItemConfig)
{
	if (ItemConfig.SpawnedItemActor != nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Get Item : %s"), *ItemConfig.SpawnedItemActor->GetFName().ToString());
		Server_GetItem(ItemConfig);
	}
	return;
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
		for (UItemDataObject* ItemDataObject : PossingItemData)
		{
			if (ItemDataObject == nullptr) continue;
			if (ItemConfig.ItemType == ItemDataObject->ItemConfig.ItemType)
			{
				ItemDataObject->ItemConfig.ItemCount++;
				ItemDataObject->OnItemSlotUpdated.Broadcast(Cast<UObject>(ItemDataObject));
				IsAdded = true;
				break;
			}
		}
	}
	if (!IsAdded)
	{
		UItemDataObject* DataObject = NewObject<UItemDataObject>(this);
		DataObject->OwningPlayer;
		DataObject->ItemConfig = ItemConfig;
		DataObject->OnItemSlotUpdated.AddUFunction(this, TEXT("OnItemDataUpdated"));
		PossingItemData.Add(DataObject);
		OnAddItemDataObject.Broadcast(Cast<UObject>(DataObject));
	}

	//PossesingItem.Add(ItemConfig);
	//if (HasAuthority()) OnRep_PossesingItem();
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
