#pragma once

#include "CoreMinimal.h"
#include "RenderResource.h"
#include "HKK_PCH.h"
#include "HKK_Structs.generated.h"

class AActor;

USTRUCT(BlueprintType)
struct FItemConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Stackable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UClass* WearableSpawnClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> HUDMaterialInstance;

	// Equipment Assignable Only
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentSlotType EquipmentSlotType;
	// Quickslot Assignable Only
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FKey QuickSlotKey = FKey("None");
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

	UPROPERTY(BlueprintReadWrite)
	float HeadingRadian;

	UPROPERTY(BlueprintReadWrite)
	FVector LocalVelocityNormalized;

	UPROPERTY(BlueprintReadWrite)
	FVector CachedInput;
};

class HKK_API HKK_Structs
{
public:

};
