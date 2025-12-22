#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IPlayerState.generated.h"

UINTERFACE(MinimalAPI)
class UIPlayerState : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IIPlayerState
{
	GENERATED_BODY()

public:
	virtual float GetCurrHP() = 0;
	virtual float GetCurrStamina() = 0;
	virtual float GetMaxHP() = 0;
	virtual float GetMaxStamina() = 0;

	virtual void SetCurrHP(float e) = 0;
	virtual void SetCurrStamina(float e) = 0;
	virtual void SetMaxHP(float e) = 0;
	virtual void SetMaxStamina(float e) = 0;
	
};
