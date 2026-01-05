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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemConfig ItemConfig;
	
	APlayerController* OwningPlayer;
	FOnItemSlotUpdated OnItemSlotUpdated;
};
