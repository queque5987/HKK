#include "Component/PickableComponent.h"
#include "Interface/IPickableItem.h"
#include "Interface/Character/ICharacterWidget.h"
#include "GameFramework/WidgetLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UPickableComponent::UPickableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPickableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPickableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPickableComponent::SetBeginOverlapEvent(TScriptInterface<class IIPickableItem> InIPickableItem)
{
	IPickable = InIPickableItem;
	if (IPickable != nullptr)
	{
		IPickable->GetComponentBeginOverlapSignature()->AddDynamic(this, &UPickableComponent::OnComponentBeginOverlap);
		IPickable->GetComponentEndOverlapSignature()->AddDynamic(this, &UPickableComponent::OnComponentEndOverlap);
	}
}

void UPickableComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickable == nullptr) return;
	AActor* CurrentPlayer = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (OtherActor == CurrentPlayer)
	{
		if (UWidgetLibrary::ItemInteractWidget(true, CurrentPlayer->GetInstigatorController(), IPickable->_getUObject(), IPickable->GetItemConfig()))
		{
			if (CurrentStencilValue == ECustomStencilValue::ECSV_Item || CurrentStencilValue == ECustomStencilValue::ECSV_ItemSelected)
			{
				CurrentStencilValue = CurrentStencilValue == ECustomStencilValue::ECSV_Item ? ECustomStencilValue::ECSV_ItemSelected : ECustomStencilValue::ECSV_Item;
				IPickable->OnItemStencilValueChange(CurrentStencilValue);
			}
		}
	}
}

void UPickableComponent::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IPickable == nullptr) return;
	AActor* CurrentPlayer = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (OtherActor == CurrentPlayer)
	{
		if (UWidgetLibrary::ItemInteractWidget(false, CurrentPlayer->GetInstigatorController(), IPickable->_getUObject(), IPickable->GetItemConfig()))
		{
			if (CurrentStencilValue == ECustomStencilValue::ECSV_Item || CurrentStencilValue == ECustomStencilValue::ECSV_ItemSelected)
			{
				CurrentStencilValue = CurrentStencilValue == ECustomStencilValue::ECSV_Item ? ECustomStencilValue::ECSV_ItemSelected : ECustomStencilValue::ECSV_Item;
				IPickable->OnItemStencilValueChange(CurrentStencilValue);
			}
		}
	}
}