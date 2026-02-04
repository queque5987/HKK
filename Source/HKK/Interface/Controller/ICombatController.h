#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ICombatController.generated.h"

class IIPlayerState;

UINTERFACE(MinimalAPI)
class UICombatController : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IICombatController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool Bind_Character(UObject* PlayerCharacterObject);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCachedInput();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetWallCoverable(bool e);
	//virtual TScriptInterface<IIPlayerState> _GetPlayerState() = 0;
};
