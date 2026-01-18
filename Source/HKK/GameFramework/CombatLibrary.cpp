#include "GameFramework/CombatLibrary.h"
#include "Interface/Character/ICharacterCombat.h"
#include "Interface/GameFramework/IPlayerState.h"
#include "Interface/Controller/IWidgetController.h"
#include "Interface/IPickableItem.h"
#include "Interface/GameFramework/IObjectPoolSystem.h"
#include "GameFramework/ObjectPoolSystem.h"

void UCombatLibrary::SendDamage(TScriptInterface<IICharacterCombat> CausedPlayer, TScriptInterface<IICharacterCombat> DamagedPlayer, FHitDamageConfig HitDamageConfig)
{
	if (CausedPlayer == nullptr)
	{
		LogWarning(3.f, TEXT("Damage Event With No Causer"));
		return;
	}
	if (DamagedPlayer == nullptr)
	{
		LogWarning(3.f, TEXT("Damaged Player Not Found"));
		return;
	}
	auto* PlayerStateObj = DamagedPlayer->GetPlayerStateObject();
	if (PlayerStateObj == nullptr)
	{
		LogWarning(3.f, TEXT("Player State Not Found"));
		return;
	}
	float CurrHP = IIPlayerState::Execute_GetCurrHP(PlayerStateObj);
	IIPlayerState::Execute_SetCurrHP(PlayerStateObj, CurrHP - HitDamageConfig.HitDamage);
	DamagedPlayer->Multicast_KnockBack(HitDamageConfig.HitDirection);
}

void UCombatLibrary::RotatePawnBasedOnControlRotation(UObject* PlayerPawnObject)
{
	if (PlayerPawnObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerPawnObject Not Found"));
		return;
	}
	IICharacterCombat::Execute_RotatePawnBasedOnControlRotation(PlayerPawnObject);
}

bool UCombatLibrary::Bind_HUD(UObject* PlayerHUDObject, UObject* PlayerStateObject)
{
	if (PlayerHUDObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerHUDObject Not Found"));
		return false;
	}
	if (PlayerStateObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerStateObject Not Found"));
		return false;
	}
	return IIPlayerState::Execute_BindDelegate_HUDWidget(PlayerStateObject, PlayerHUDObject);
}

bool UCombatLibrary::Bind_Inventory(UObject* PlayerInventoryObject, UObject* PlayerStateObject)
{
	if (PlayerInventoryObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerInventoryObject Not Found"));
		return false;
	}
	if (PlayerStateObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerStateObject Not Found"));
		return false;
	}
	return IIPlayerState::Execute_BindDelegate_InventoryWidget(PlayerStateObject, PlayerInventoryObject);
}

bool UCombatLibrary::Bind_Equipment(UObject* PlayerCharacterObject, UObject* PlayerStateObject)
{
	if (PlayerCharacterObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerCharacterObject Not Found"));
		return false;
	}
	if (PlayerStateObject == nullptr)
	{
		LogWarning(3.f, TEXT("PlayerStateObject Not Found"));
		return false;
	}
	return IIPlayerState::Execute_BindDelegate_Equipment(PlayerStateObject, PlayerCharacterObject);
}

bool UCombatLibrary::EquipItem(UObject* OwningPlayerController, UObject* EquipItemDataObject, FItemConfig EquipItemConfig)
{
	if (EquipItem == nullptr || OwningPlayerController == nullptr) return false;

	IIWidgetController::Execute_ChangeEquipSlot(OwningPlayerController, EquipItemDataObject, EquipItemConfig.EquipmentSlotType);
	//UE_LOG(LogTemp, Log, TEXT("Attaching Item %s To %s"), *EquipItem->GetFName().ToString(), *OwningPlayerController->GetFName().ToString());
	return true;
}

bool UCombatLibrary::AttachItem(UObject* OwningPlayerCharacter, AActor* AttachItemActor, FName AttachSocketName)
{
	if (AttachItemActor != nullptr) IIPickableItem::Execute_SetPickableItem(AttachItemActor, false);
	return IICharacterCombat::Execute_AttachItem(OwningPlayerCharacter, AttachItemActor, AttachSocketName);
}

bool UCombatLibrary::DetachItem(UObject* OwningPlayerCharacter, AActor* AttachItemActor)
{
	return IICharacterCombat::Execute_DetachItem(OwningPlayerCharacter, AttachItemActor);
}

bool UCombatLibrary::GetItem(UObject* ItemOwnerPlayerState, const FItemConfig& ItemConfig)
{
	IIPlayerState::Execute_GetItem(ItemOwnerPlayerState, ItemConfig);
	return true;
}

AActor* UCombatLibrary::SpawnFromPool(const UObject* WorldObject, UClass* SpawnActorClass)
{
	if (WorldObject == nullptr) return nullptr;
	UWorld* W = WorldObject->GetWorld();
	if (W == nullptr) return nullptr;
	UObjectPoolSystem* ObjectPoolSystem = W->GetSubsystem<UObjectPoolSystem>();
	if (ObjectPoolSystem == nullptr) return nullptr;
	return ObjectPoolSystem->GetFromPool(SpawnActorClass);
}

bool UCombatLibrary::ReturnToPool(const UObject* WorldObject, UClass* ReturnActorClass, AActor* ReturnActor)
{
	if (WorldObject == nullptr) return false;
	UWorld* W = WorldObject->GetWorld();
	if (W == nullptr) return false;
	UObjectPoolSystem* ObjectPoolSystem = W->GetSubsystem<UObjectPoolSystem>();
	if (ObjectPoolSystem == nullptr) return false;
	return ObjectPoolSystem->ReturnToPool(ReturnActorClass, ReturnActor);
}

//bool UCombatLibrary::RefreshQuickSlot(UObject* OwningPlayerController, UObject* ChangedItemDataObject)
//{
//	if (OwningPlayerController == nullptr)
//	{
//		LogWarning(3.f, TEXT("OwningPlayerController Not Found"));
//		return false;
//	}
//	if (ChangedItemDataObject == nullptr)
//	{
//		LogWarning(3.f, TEXT("ChangedItemDataObject Not Found"));
//		return false;
//	}
//	IIWidgetController::Execute_RefreshQuickSlot(OwningPlayerController, ChangedItemDataObject);
//	return false;
//}
