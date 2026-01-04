#include "GameFramework/HKK_PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Interface/Widget/IUpdatableWidget.h"

void AHKK_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHKK_PlayerState, CurrHP);
	DOREPLIFETIME(AHKK_PlayerState, MaxHP);
	DOREPLIFETIME(AHKK_PlayerState, CurrStamina);
	DOREPLIFETIME(AHKK_PlayerState, MaxStamina);
	DOREPLIFETIME(AHKK_PlayerState, PossesingItem);
}

bool AHKK_PlayerState::BindDelegate_HUDWidget_Implementation(UObject* BindWidget)
{
	if (BindWidget == nullptr) return false;
	OnUpdateStatFloat.AddUFunction(BindWidget, "OnUpdatePlayerStatFloat");
	IIUpdatableWidget::Execute_OnUpdatePlayerStatFloat(BindWidget, EPlayerStatType::EPST_HP, 1.f, 1.f);
	return OnUpdateStatFloat.IsBoundToObject(BindWidget);
}

bool AHKK_PlayerState::BindDelegate_InventoryWidget_Implementation(UObject* BindWidget)
{
	if (BindWidget == nullptr) return false;
	OnUpdateItemToInventory.AddUFunction(BindWidget, "AddItem");
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
	PossesingItem.Add(ItemConfig);
	if (HasAuthority()) OnRep_PossesingItem();
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

void AHKK_PlayerState::OnRep_PossesingItem()
{
	if (OnUpdateItemToInventory.IsBound())
	{
		UE_LOG(LogTemp, Log, TEXT("OnRep_PossesingItem Is Bound To Object"));
	}
	else 
	{
		UE_LOG(LogTemp, Log, TEXT("OnRep_PossesingItem Is Not Bound To Object"));
	}
	OnUpdateItemToInventory.Broadcast(true, EUserWidget::EUW_Inventory, PossesingItem.Last());
}
