#pragma once

#include "CoreMinimal.h"
#include "HKK_Structs.generated.h"

USTRUCT(BlueprintType)
struct FCharacterMovementState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;

	UPROPERTY(BlueprintReadWrite)
	float FacingYaw;
};

class HKK_API HKK_Structs
{
public:

};
