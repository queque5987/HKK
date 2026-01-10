#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetLibrary.generated.h"

UCLASS()
class HKK_API UWidgetLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void QuickSlotChanged(UObject* ChangedPlayerController, UObject* ChangedItemObject, FKey ChangedKey);
	
	
	
};
