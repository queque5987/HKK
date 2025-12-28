#pragma once

#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "FoliageInteractSourceComponent.generated.h"

class AFoliageInteractVolume;
class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HKK_API UFoliageInteractSourceComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UFoliageInteractSourceComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Collider;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AFoliageInteractVolume*> ToCheckFoliageVolumes;

	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
	
};
