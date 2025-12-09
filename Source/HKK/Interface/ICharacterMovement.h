#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_Structs.h"
#include "ICharacterMovement.generated.h"

UINTERFACE(MinimalAPI)
class UICharacterMovement : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IICharacterMovement
{
	GENERATED_BODY()

public:
	
	virtual FVector _GetVelocity() { return FVector(); };

	virtual FCharacterMovementState* GetCharacterMovementState() { return nullptr; };
};
