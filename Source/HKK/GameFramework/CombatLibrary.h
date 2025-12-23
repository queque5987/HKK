#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HKK_Structs.h"
#include "CombatLibrary.generated.h"

class IICharacterCombat;

UCLASS()
class HKK_API UCombatLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static void SendDamage(TScriptInterface<IICharacterCombat> CausedPlayer, TScriptInterface<IICharacterCombat> DamagedPlayer, FHitDamageConfig HitDamageConfig);

	// sudo
	static bool Bind_HUD(UObject* PlayerHUDObject, UObject* PlayerStateObject);

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
