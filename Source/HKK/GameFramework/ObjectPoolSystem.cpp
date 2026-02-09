#include "GameFramework/ObjectPoolSystem.h"
#include "Blueprint/UserWidget.h"
#include "Interface/Controller/IWidgetController.h"

AActor* UObjectPoolSystem::GetFromPool(UClass* InClass)
{
	TSharedPtr<FActorPool>* ClassActorPoolPtr = ObjectPool.Find(InClass);
	if (ClassActorPoolPtr) UE_LOG(LogTemp, Log, TEXT("Class: %s | Pool Size: %d"), *InClass->GetName(), (*ClassActorPoolPtr)->Num());
	if (!ClassActorPoolPtr)
	{
		ClassActorPoolPtr = &(ObjectPool.Add(InClass, MakeShared<FActorPool>()));
	}
	TObjectPtr<AActor> OutActor;
	if ((*ClassActorPoolPtr)->Pop(OutActor))
	{
		if (IsValid(OutActor))
		{
			return OutActor;
		}
	}
	return GetWorld()->SpawnActor<AActor>(InClass);
}

bool UObjectPoolSystem::ReturnToPool(UClass* InClass, AActor* InActor)
{
	if (!IsValid(InActor)) return false;

	TSharedPtr<FActorPool>* ClassActorPoolPtr = ObjectPool.Find(InClass);
	if (!ClassActorPoolPtr)
	{
		ClassActorPoolPtr = &(ObjectPool.Add(InClass, MakeShared<FActorPool>()));
	}
	InActor->SetActorLocation(FVector(0.f, 0.f, -99999.f));
	(*ClassActorPoolPtr)->Push(InActor);
	return true;
}

UUserWidget* UObjectPoolSystem::GetWidgetFromPool(TSubclassOf<UUserWidget> InClass, UObject* PlayerControllerObject)
{
	TSharedPtr<FWidgetPool>* ClassWidgetPoolPtr = WidgetPool.Find(InClass);
	if (ClassWidgetPoolPtr) UE_LOG(LogTemp, Log, TEXT("Class: %s | Pool Size: %d"), *InClass->GetName(), (*ClassWidgetPoolPtr)->Num());
	if (!ClassWidgetPoolPtr)
	{
		ClassWidgetPoolPtr = &(WidgetPool.Add(InClass, MakeShared<FWidgetPool>()));
	}
	TObjectPtr<UUserWidget> OutWidget;
	if ((*ClassWidgetPoolPtr)->Pop(OutWidget))
	{
		if (IsValid(OutWidget))
		{
			return OutWidget;
		}
	}
	return IIWidgetController::Execute_CreateSimpleWidget(PlayerControllerObject, InClass);
}

bool UObjectPoolSystem::ReturnWidgetToPool(UClass* InClass, UUserWidget* InWidget)
{
	if (!IsValid(InWidget)) return false;

	TSharedPtr<FWidgetPool>* ClassWidgetPoolPtr = WidgetPool.Find(InClass);
	if (!ClassWidgetPoolPtr)
	{
		ClassWidgetPoolPtr = &(WidgetPool.Add(InClass, MakeShared<FWidgetPool>()));
	}
	InWidget->SetVisibility(ESlateVisibility::Collapsed);

	(*ClassWidgetPoolPtr)->Push(InWidget);
	return true;
}