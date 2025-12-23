#include "Component/PickableComponent.h"
#include "Interface/IPickableItem.h"
#include "Interface/Character/ICharacterWidget.h"
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
		IICharacterWidget* CharacterWidget = Cast<IICharacterWidget>(OtherActor);
		if (CharacterWidget != nullptr)
		{
			CharacterWidget->SetItemInteractWidget(true, IPickable, IPickable->GetItemConfig());
		}

		CurrentStencilValue = CurrentStencilValue == ECustomStencilValue::ECSV_Item ? ECustomStencilValue::ECSV_ItemSelected : ECustomStencilValue::ECSV_Item;
		IPickable->OnItemStencilValueChange(CurrentStencilValue);
	}
}

void UPickableComponent::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IPickable == nullptr) return;
	AActor* CurrentPlayer = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (OtherActor == CurrentPlayer)
	{
		IICharacterWidget* CharacterWidget = Cast<IICharacterWidget>(OtherActor);
		if (CharacterWidget != nullptr)
		{
			CharacterWidget->SetItemInteractWidget(false, IPickable, IPickable->GetItemConfig());
		}
		CurrentStencilValue = CurrentStencilValue == ECustomStencilValue::ECSV_Item ? ECustomStencilValue::ECSV_ItemSelected : ECustomStencilValue::ECSV_Item;
		IPickable->OnItemStencilValueChange(CurrentStencilValue);
	}
}