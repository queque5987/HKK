#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HKK_Structs.h"
#include "CombatLibrary.generated.h"

class IICharacterCombat;
class AActor;

UCLASS()
class HKK_API UCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SendDamage(TScriptInterface<IICharacterCombat> CausedPlayer, TScriptInterface<IICharacterCombat> DamagedPlayer, FHitDamageConfig HitDamageConfig);
	static bool Bind_HUD(UObject* PlayerHUDObject, UObject* PlayerStateObject);
	static bool Bind_Inventory(UObject* PlayerInventoryObject, UObject* PlayerStateObject);
	static bool Bind_Equipment(UObject* PlayerCharacterObject, UObject* PlayerStateObject);

	/* Widget Only */
	static bool EquipItem(UObject* OwningPlayerController, UObject* EquipItemDataObject, FItemConfig EquipItemConfig);
	static bool AttachItem(UObject* OwningPlayerCharacter, AActor* AttachItemActor, FName AttachSocketName);
	static bool GetItem(UObject* ItemOwnerPlayerState, const FItemConfig& ItemConfig);
	//static bool RefreshQuickSlot(UObject* OwningPlayerController, UObject* ChangedItemDataObject);
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
