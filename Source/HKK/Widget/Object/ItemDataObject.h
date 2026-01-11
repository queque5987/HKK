#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HKK_Delegates.h"
#include "HKK_Structs.h"
#include "ItemDataObject.generated.h"

class APlayerController;

UCLASS(Blueprintable, BlueprintType)
class HKK_API UItemDataObject : public UObject
{
	GENERATED_BODY()

protected:
	bool IsQuickSlotInitializer = false;
	bool EmptySlot = false;
	FKey QuickSlotKey;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemConfig ItemConfig;
	
	APlayerController* OwningPlayer;
	FOnItemSlotUpdated OnItemSlotUpdated;

	bool GetIsQuickSlotInitializer() { return IsQuickSlotInitializer; };
	void SetIsQuickSlotInitializer(FKey Key) { 
		IsQuickSlotInitializer = (Key != FKey("None") ? true : false);
		QuickSlotKey = Key;
	};
	void UseAsEmptySlot()
	{
		EmptySlot = true;
	};
	FKey GetQuickSlotKey() { return QuickSlotKey; };
	bool IsEmptySlot() { return EmptySlot; };
};
