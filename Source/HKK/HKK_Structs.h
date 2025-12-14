#pragma once

#include "CoreMinimal.h"
#include "HKK_Structs.generated.h"

USTRUCT(BlueprintType)
struct FHitDamageConfig
{
	GENERATED_BODY()
	float HitDamage;
	FVector HitDirection;
	FVector HitLocation;

};
USTRUCT(BlueprintType)
struct FHitTraceConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TraceStartBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TraceEndBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitDamage;

	USkeletalMeshComponent* TraceMesh;

};
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
