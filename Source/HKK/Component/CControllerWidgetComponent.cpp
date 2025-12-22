#include "Component/CControllerWidgetComponent.h"
#include "Interface/Controller/IWidgetController.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/CWidget_ItemInteract.h"
#include "Widget/CWidget_HUD.h"

UCControllerWidgetComponent::UCControllerWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	WidgetFloating = 0;
}

void UCControllerWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UCControllerWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector2D ProjectLocation = FVector2D::ZeroVector;
	const APlayerController* PC = Cast<APlayerController>(WidgetController->_getUObject());
	//UGameplayStatics::ProjectWorldToScreen(PC, WidgetController->GetPlayerLocation(), ProjectLocation);
	int32 ScreensizeX;
	int32 ScreensizeY;
	PC->GetViewportSize(ScreensizeX, ScreensizeY);
	if (WidgetFloating & 1 << (uint8)EUserWidget::EUW_ItemInteract)
	{
		if (InteractingPickableItem != nullptr)
		{
			UGameplayStatics::ProjectWorldToScreen(PC, InteractingPickableItem->GetActorLocation(), ProjectLocation);
			//ProjectLocation.X += ScreensizeX / 12.f; TODO; X + Get Actor Size?
		}
		Widget_ItemInteract->SetPositionInViewport(ProjectLocation);
	}
}

void UCControllerWidgetComponent::SetController(TScriptInterface<IIWidgetController> InWidgetController)
{
	WidgetController = InWidgetController;

	if (WidgetController == nullptr) return;

	WidgetController->GetOnSetItemInteractPickupWidget().AddUFunction(this, TEXT("SetItemInteractPickupWidget"));
	WidgetController->GetOnKeyTriggered().AddUFunction(this, TEXT("Callback_OnKeyTriggered"));
	WidgetController->GetOnKeyReleased().AddUFunction(this, TEXT("Callback_OnKeyReleased"));
	OnGetItem = &WidgetController->GetOnGetItem();
	OnGetItem->AddLambda([this](const FItemConfig& ItemConfig, AActor* OwningActor)
		{
			SetItemInteractPickupWidget(false, EUserWidget::EUW_ItemInteract, ItemConfig);
		}
	);
	if (Widget_ItemInteract != nullptr)
	{
		Widget_ItemInteract->AddToViewport();
		Widget_ItemInteract->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (Widget_HUD != nullptr)
	{
		Widget_HUD->AddToViewport();
		Widget_HUD->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UCControllerWidgetComponent::SetItemInteractPickupWidget(bool ToSet, EUserWidget WidgetType, const FItemConfig& ItemConfig)
{
	FVector2D ProjectLocation;
	InteractingPickableItem = ItemConfig.SpawnedItemActor;
	if (WidgetController == nullptr || WidgetController == nullptr)
	{
		Widget_ItemInteract->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	const APlayerController* PC = Cast<APlayerController>(WidgetController->_getUObject());
	UGameplayStatics::ProjectWorldToScreen(PC, WidgetController->GetPlayerLocation(), ProjectLocation);
	int32 ScreensizeX;
	int32 ScreensizeY;
	PC->GetViewportSize(ScreensizeX, ScreensizeY);
	if (ToSet) WidgetFloating |= 1 << (uint8)EUserWidget::EUW_ItemInteract;
	else WidgetFloating &= ~1 << (uint8)EUserWidget::EUW_ItemInteract;

	UE_LOG(LogTemp, Log, TEXT("UCControllerWidgetComponent::SetItemInteractPickupWidget Location : %s"), *ProjectLocation.ToString());
	ProjectLocation.X += ScreensizeX / 12.f;
	Widget_ItemInteract->SetPositionInViewport(ProjectLocation);
	Widget_ItemInteract->SetVisibility(ToSet ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	Widget_ItemInteract->SetItemConfig(OnGetItem, ItemConfig);
}

void UCControllerWidgetComponent::Callback_OnKeyTriggered(FKey Key)
{
	if (Key.GetFName() == FName("E") && Widget_ItemInteract != nullptr)
	{
		Widget_ItemInteract->SwitchWidget(true);
	}
}

void UCControllerWidgetComponent::Callback_OnKeyReleased(FKey Key)
{
	if (Key.GetFName() == FName("E") && Widget_ItemInteract != nullptr)
	{
		Widget_ItemInteract->SwitchWidget(false);
	}
}

