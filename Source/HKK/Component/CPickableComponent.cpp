#include "Component/CPickableComponent.h"
#include "Interface/IPickableItem.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UCPickableComponent::UCPickableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCPickableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCPickableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCPickableComponent::SetBeginOverlapEvent(TScriptInterface<class IIPickableItem> InIPickableItem)
{
	IPickable = InIPickableItem;
	if (IPickable != nullptr)
	{
		IPickable->GetComponentBeginOverlapSignature()->AddDynamic(this, &UCPickableComponent::OnComponentBeginOverlap);
		IPickable->GetComponentEndOverlapSignature()->AddDynamic(this, &UCPickableComponent::OnComponentEndOverlap);
	}
}

void UCPickableComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickable == nullptr) return;
	AActor* CurrentPlayer = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (OtherActor == CurrentPlayer)
	{
		CurrentStencilValue = CurrentStencilValue == ECustomStencilValue::ECSV_Item ? ECustomStencilValue::ECSV_ItemSelected : ECustomStencilValue::ECSV_Item;
		IPickable->OnItemStencilValueChange(CurrentStencilValue);
	}
	//else
	//{
	//	CurrentStencilValue = ECSV_ItemOccupied;
	//	IPickable->OnItemStencilValueChange(CurrentStencilValue);
	//}
}

void UCPickableComponent::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IPickable == nullptr) return;
	AActor* CurrentPlayer = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (OtherActor == CurrentPlayer)
	{
		CurrentStencilValue = CurrentStencilValue == ECustomStencilValue::ECSV_Item ? ECustomStencilValue::ECSV_ItemSelected : ECustomStencilValue::ECSV_Item;
		IPickable->OnItemStencilValueChange(CurrentStencilValue);
	}
	//else
	//{
	//	CurrentStencilValue = ECustomStencilValue::ECSV_Item;
	//	IPickable->OnItemStencilValueChange(CurrentStencilValue);
	//}
}