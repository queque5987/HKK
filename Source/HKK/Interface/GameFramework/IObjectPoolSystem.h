#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IObjectPoolSystem.generated.h"

UINTERFACE(MinimalAPI)
class UIObjectPoolSystem : public UInterface
{
	GENERATED_BODY()
};

class AActor;
class UUserWidget;

class HKK_API IIObjectPoolSystem
{
	GENERATED_BODY()

public:
	
	virtual AActor* GetFromPool(UClass* InClass) { return nullptr; };
	virtual bool ReturnToPool(UClass* InClass, AActor* InActor) { return bool(); };
	virtual UUserWidget* GetWidgetFromPool(TSubclassOf<UUserWidget> InClass, UObject* PlayerControllerObject) { return nullptr; };
	virtual bool ReturnWidgetToPool(UClass* InClass, UUserWidget* InWidget) { return bool(); };
};
