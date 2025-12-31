#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FoliageInteractVolume.generated.h"

class UBoxComponent;
class UMaterialInstance;
class UMaterialInstanceDynamic;
class UTextureRenderTarget2D;
class IICharacterMovement;

UCLASS()
class HKK_API AFoliageInteractVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AFoliageInteractVolume();

protected:
	/*Box Should Be 1:1:1 Size For Now*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> FoliageInteractionBound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstance> MI_FoliageInteractRenderTargetGenerator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstanceDynamic> MID_FoliageInteractRenderTargetGenerator;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextureRenderTarget2D> RT_FoliageInteractImpact;

	TQueue<TScriptInterface<IICharacterMovement>> InteractingSourceComponentQueue;
	TSet<TScriptInterface<IICharacterMovement>> InteractingSourceComponentSet;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UTextureRenderTarget2D> RT_FoliageInteractImpact_B;

	
	bool bFlipRT;
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	bool IsInside(const FVector& Location);
	void SetImpulse(const FVector& ImpactLocation, float Radius);
	void PushInteractSourceComponent(TScriptInterface<IICharacterMovement>& InSourceComponent);

private:
	void SetImpulse(TScriptInterface<IICharacterMovement>& InSourceComponent, int8 SlotIndex);
};
