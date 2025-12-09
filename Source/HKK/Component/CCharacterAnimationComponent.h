#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCharacterAnimationComponent.generated.h"


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
	TObjectPtr<class UAnimSequence> Animation_Attack_RFist;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
	
	class UAnimSequence* GetAnimationSequence(int8 AnimationType);
};
