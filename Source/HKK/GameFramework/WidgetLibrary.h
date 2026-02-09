#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HKK_PCH.h"
#include "HKK_Structs.h"
#include "WidgetLibrary.generated.h"

class UUserWidget;

UCLASS()
class HKK_API UWidgetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool ItemInteractWidget(bool IsOn, UObject* OwningPlayerController, UObject* PickableItemObject, const FItemConfig& ItemConfig);
	static void QuickSlotChanged(UObject* ChangedPlayerController, UObject* ChangedItemObject, FKey ChangedKey);
	static void EquipmentSlotChanged(UObject* ChangedPlayerController, UObject* ChangedItemObject, EEquipmentSlotType EquipmentSlotType);
	static void EquipmentItemDragDetected(UObject* OwningPlayerController, bool e);
	static EEquipmentSlotType GetLeftEquipmentSlotIndex(UObject* OwningPlayerController);
	static UUserWidget* GetWidget(UObject* OwningPlayerControllerObject, TSubclassOf<UUserWidget> CreateWidgetClass);
	static bool ReturnWidget(UObject* WorldContextObject, TSubclassOf<UUserWidget> ReturnWidgetClass, UUserWidget* InWidget);

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
