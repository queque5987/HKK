#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_PCH.h"
#include "IUpdatableWidget.generated.h"

UINTERFACE(MinimalAPI)
class UIUpdatableWidget : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IIUpdatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnUpdatePlayerStatFloat(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat);
	
};
