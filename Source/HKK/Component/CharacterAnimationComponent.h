#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HKK_PCH.h"
#include "CharacterAnimationComponent.generated.h"

class UAnimSequence;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class HKK_API UCharacterAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAnimationComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<EPlayerAnimation, TObjectPtr<UAnimSequence>> AnimationMap;

	UPROPERTY(ReplicatedUsing = OnRep_WallCoverableObjectCheck)
	uint8 WallCoverableObjectCheck = 0;
	UPROPERTY(Replicated)
	FVector WallCoverNormalVector;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
	
	UAnimSequence* GetAnimationSequence(const EPlayerAnimation PlayerAnimation);

	UFUNCTION()
	void OnRep_WallCoverableObjectCheck();
	UFUNCTION()
	FVector GetWallCoverNormal() { return WallCoverNormalVector; };;
private:
	UFUNCTION(Server, Reliable)
	void Server_ForwardLineTrace_WallCover();
	FVector GetMostNormalVector(const TArray<FVector_NetQuantizeNormal>& Normals);
};
