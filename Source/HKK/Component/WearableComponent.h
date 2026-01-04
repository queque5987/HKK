#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/Item/IWearableComponent.h"
#include "WearableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HKK_API UWearableComponent : public UActorComponent, public IIWearableComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWearableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual class USceneComponent* GetAttachComponent() override;
	
};
