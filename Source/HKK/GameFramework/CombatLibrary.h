#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HKK_Structs.h"
#include "HKK_PCH.h"
#include "CombatLibrary.generated.h"

class IICharacterCombat;
class AActor;

UCLASS()
class HKK_API UCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SendDamage(TScriptInterface<IICharacterCombat> CausedPlayer, TScriptInterface<IICharacterCombat> DamagedPlayer, FHitDamageConfig HitDamageConfig);
	static void RotatePawnBasedOnControlRotation(UObject* PlayerPawnObject);

	static bool Bind_Character(UObject* PlayerControllerObject, UObject* PlayerCharacterObject);
	static bool Bind_HUD(UObject* PlayerHUDObject, UObject* PlayerStateObject);
	static bool Bind_Inventory(UObject* PlayerInventoryObject, UObject* PlayerStateObject);
	static bool Bind_Equipment(UObject* PlayerCharacterObject, UObject* PlayerStateObject);

	/* Widget Only */
	static bool EquipItem(UObject* OwningPlayerController, UObject* EquipItemDataObject, FItemConfig EquipItemConfig);
	static bool AttachItem(UObject* OwningPlayerCharacter, AActor* AttachItemActor, FName AttachSocketName);
	static bool DetachItem(UObject* OwningPlayerCharacter, AActor* AttachItemActor);
	static bool GetItem(UObject* ItemOwnerPlayerState, const FItemConfig& ItemConfig);
	//static bool RefreshQuickSlot(UObject* OwningPlayerController, UObject* ChangedItemDataObject);
	static AActor* SpawnFromPool(const UObject* WorldObject, UClass* SpawnActorClass);
	static bool ReturnToPool(const UObject* WorldObject, UClass* ReturnActorClass, AActor* ReturnActor);

	/* Atleast 1 Item Equipped */
	static bool CanWeaponScrollSwitch(UObject* OwningPlayerState);

	static void AnimInstance_SetBoolValue(UObject* OwningPlayerAnimInatceObject, EPlayerState ToSetPlayerState, bool e);
private:
	template<typename...Args>
	static void LogWarning(float DisplayTime, const FString& LogText)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *LogText);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, DisplayTime, FColor::Orange, *LogText);
		}
	};
};
