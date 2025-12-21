#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HKK_Delegates.h"
#include "HKK_Structs.h"
#include "HKK_PCH.h"
#include "InputCoreTypes.h"
#include "CControllerWidgetComponent.generated.h"

class IIWidgetController;
class UCWidget_ItemInteract;
class AActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HKK_API UCControllerWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCControllerWidgetComponent();

protected:
	virtual void BeginPlay() override;

	TScriptInterface<IIWidgetController> WidgetController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCWidget_ItemInteract> Widget_ItemInteract;
	UINT16 WidgetFloating;

	TObjectPtr<AActor> InteractingPickableItem;
	FOnGetItem* OnGetItem;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetController(TScriptInterface<IIWidgetController> InWidgetController);
	
	UFUNCTION()
	void SetItemInteractPickupWidget(bool ToSet, EUserWidget WidgetType, const FItemConfig& ItemConfig);
	UFUNCTION()
	void Callback_OnKeyTriggered(const FKey Key);
	UFUNCTION()
	void Callback_OnKeyReleased(const FKey Key);
};
