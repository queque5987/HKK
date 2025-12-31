#pragma once

#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Interface/ICharacterMovement.h"
#include "FoliageInteractSourceComponent.generated.h"

class AFoliageInteractVolume;
class USphereComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HKK_API UFoliageInteractSourceComponent : public USceneComponent, public IICharacterMovement
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float VelocityUpdateInterval = 0.1f;

	FName SocketName;
	FTimerHandle VelocityUpdateTimerHandle;
	FVector PreviousBoneLocation;
	FVector CachedVelocity;
	TWeakObjectPtr<USkeletalMeshComponent> CachedSkelMeshParent;
	TWeakObjectPtr<UStaticMeshComponent> CachedStaticMeshParent;

	void UpdateSocketVelocity();
	virtual void OnAttachmentChanged() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	UFoliageInteractSourceComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Collider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AFoliageInteractVolume*> ToCheckFoliageVolumes;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))

	TObjectPtr<AFoliageInteractVolume> CurrentOverlappingVolume;

	bool bEneable = true;

	float ColliderRadius = 50.f;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool AttachToComponent(USceneComponent* InParent, const FAttachmentTransformRules& AttachmentRules, FName InSocketName = NAME_None) override;
	
	virtual FVector _GetVelocity() override;
	virtual FVector _GetLocation() override;
	virtual float GetComponentCollideRadius() override;

};
