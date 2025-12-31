#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UStaticMeshComponent;
class UFoliageInteractSourceComponent;

UCLASS()
class HKK_API AProjectile : public AActor
{
	GENERATED_BODY()
public:	
	AProjectile();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UFoliageInteractSourceComponent> FoliageInteractSourceComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DebugProjectSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DebugLifeTime;
	float DebugCurrentLifeTime;
	virtual void BeginPlay() override;
public:	
	virtual void Tick(float DeltaTime) override;
};
