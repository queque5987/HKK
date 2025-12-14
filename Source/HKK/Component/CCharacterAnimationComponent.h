#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HKK_PCH.h"
#include "CCharacterAnimationComponent.generated.h"

class UAnimSequence;

//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class HKK_API UCCharacterAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCCharacterAnimationComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<EPlayerAnimation, TObjectPtr<UAnimSequence>> AnimationMap;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
	
	UAnimSequence* GetAnimationSequence(const EPlayerAnimation PlayerAnimation);
};
