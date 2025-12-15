#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HKK_Delegates.h"
#include "HKK_PCH.h"
#include "CPickableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HKK_API UCPickableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCPickableComponent();

protected:
	virtual void BeginPlay() override;

	ECustomStencilValue CurrentStencilValue = ECustomStencilValue::ECSV_Item;
	TScriptInterface<class IIPickableItem> IPickable;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetBeginOverlapEvent(TScriptInterface<class IIPickableItem> InIPickableItem);

	UFUNCTION()
	void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
