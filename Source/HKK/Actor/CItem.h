#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HKK_PCH.h"
#include "HKK_Delegates.h"
#include "Interface/IPickableItem.h"
#include "CItem.generated.h"

UCLASS()
class HKK_API ACItem : public AActor, public IIPickableItem
{
	GENERATED_BODY()
	
public:	
	ACItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCPickableComponent* PickableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCWearableComponent* WearableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bFloating;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bRotating;

	FVector SMCurrentRelativeLocation;

	bool bFloatingUp = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float fMaxFloatingHeight = 50.f;
	float fElipsedTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float fFloatingSpeed = 0.85f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float fRotatingSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FItemConfig ItemConfig;
	virtual void BeginPlay() override;

	FOnChangeStencilValue OnChangeStencilValue;
public:	
	virtual void Tick(float DeltaTime) override;
	/*
----- Pickable Item Start
	*/
	virtual void OnItemStencilValueChange(ECustomStencilValue CustomStencilValue) override;
	virtual FComponentBeginOverlapSignature* GetComponentBeginOverlapSignature() override;
	virtual FComponentEndOverlapSignature* GetComponentEndOverlapSignature() override;
	virtual FORCEINLINE const FItemConfig& GetItemConfig() override { return ItemConfig; };
	/*
----- Pickable Item End
	*/
};
