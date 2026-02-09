#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interface/GameFramework/IObjectPoolSystem.h"
#include "HKK_Structs.h"
#include "ObjectPoolSystem.generated.h"

UCLASS()
class HKK_API UObjectPoolSystem : public UWorldSubsystem, public IIObjectPoolSystem // 인터페이스 외쓰지
{
	GENERATED_BODY()

    //struct FActorPool
    //{
    //    TQueue<TObjectPtr<AActor>> PoolQueue;
    //    int32 CurrentSize = 0;

    //    bool Enqueue(TObjectPtr<AActor> Actor)
    //    {
    //        if (PoolQueue.Enqueue(Actor))
    //        {
    //            CurrentSize++;
    //            return true;
    //        }
    //        return false;
    //    }

    //    bool Dequeue(TObjectPtr<AActor>& OutActor)
    //    {
    //        if (PoolQueue.Dequeue(OutActor))
    //        {
    //            CurrentSize--;
    //            return true;
    //        }
    //        return false;
    //    }
    //};

	TMap<UClass*, TSharedPtr<FActorPool>> ObjectPool;
	TMap<UClass*, TSharedPtr<FWidgetPool>> WidgetPool;
	
public:
	virtual AActor* GetFromPool(UClass* InClass) override;
	virtual bool ReturnToPool(UClass* InClass, AActor* InActor) override;
	virtual UUserWidget* GetWidgetFromPool(TSubclassOf<UUserWidget> InClass, UObject* PlayerControllerObject) override;
	virtual bool ReturnWidgetToPool(UClass* InClass, UUserWidget* InWidget) override;
};
