#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Interface/GameFramework/IObjectPoolSystem.h"
#include "ObjectPoolSystem.generated.h"

UCLASS()
class HKK_API UObjectPoolSystem : public UWorldSubsystem, public IIObjectPoolSystem
{
	GENERATED_BODY()

    struct FActorPool
    {
        TQueue<TObjectPtr<AActor>> PoolQueue;
        int32 CurrentSize = 0;

        bool Enqueue(TObjectPtr<AActor> Actor)
        {
            if (PoolQueue.Enqueue(Actor))
            {
                CurrentSize++;
                return true;
            }
            return false;
        }

        bool Dequeue(TObjectPtr<AActor>& OutActor)
        {
            if (PoolQueue.Dequeue(OutActor))
            {
                CurrentSize--;
                return true;
            }
            return false;
        }
    };

	//typedef TQueue<TObjectPtr<AActor>> FActorPool;
	TMap<UClass*, TSharedPtr<FActorPool>> ObjectPool;
	
public:
	virtual AActor* GetFromPool(UClass* InClass) override;
	virtual bool ReturnToPool(UClass* InClass, AActor* InActor) override;
};
