#include "Component/CControllerWidgetComponent.h"
#include "Interface/Controller/IWidgetController.h"
#include "Widget/CWidget_ItemInteract.h"

UCControllerWidgetComponent::UCControllerWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCControllerWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCControllerWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCControllerWidgetComponent::SetController(TScriptInterface<IIWidgetController> InWidgetController)
{
	WidgetController = InWidgetController;

	if (WidgetController == nullptr) return;

	WidgetController->GetOnSetWidget()->AddUFunction(this, TEXT("SetWidget"));

	if (Widget_ItemInteract != nullptr)
	{
		Widget_ItemInteract->AddToViewport();
		Widget_ItemInteract->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCControllerWidgetComponent::SetWidget(bool ToSet, EUserWidget WidgetType)
{
	Widget_ItemInteract->SetVisibility(ToSet ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

