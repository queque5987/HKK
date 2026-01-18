#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_PCH.h"
#include "HKK_Structs.h"
#include "IPickableItem.generated.h"

UINTERFACE(MinimalAPI)
class UIPickableItem : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IIPickableItem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool GetPickableItem();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPickableItem(bool e);

	virtual void OnItemStencilValueChange(ECustomStencilValue CustomStencilValue) {};
	virtual FComponentBeginOverlapSignature* GetComponentBeginOverlapSignature() { return nullptr; };
	virtual FComponentEndOverlapSignature* GetComponentEndOverlapSignature() { return nullptr; };
	virtual const FItemConfig& GetItemConfig() = 0;
};
