#pragma once

#include "CoreMinimal.h"
#include "RenderResource.h"
#include "HKK_Structs.generated.h"

class AActor;

USTRUCT(BlueprintType)
struct FItemConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> ItemIcon;

	TObjectPtr<AActor> SpawnedItemActor;
};
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

	TObjectPtr<USkeletalMeshComponent> TraceMesh;

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
