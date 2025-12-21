#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HKK_Delegates.h"
#include "IWidgetController.generated.h"

UINTERFACE(MinimalAPI)
class UIWidgetController : public UInterface
{
	GENERATED_BODY()
};

class HKK_API IIWidgetController
{
	GENERATED_BODY()

public:

	virtual FOnSetWidget* GetOnSetWidget() { return nullptr; }
};
