#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_Structs.h"
#include "ICharacterWidget.generated.h"

UINTERFACE(MinimalAPI)
class UICharacterWidget : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IICharacterWidget
{
	GENERATED_BODY()

public:
	
	virtual void SetItemInteractWidget(bool ToSet, TScriptInterface<class IIPickableItem> PickableItem, const FItemConfig& ItemConfig) {};
};
