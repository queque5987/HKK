#include "Actor/FoliageInteractVolume.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Interface/ICharacterMovement.h"
#include "Components/BoxComponent.h"

AFoliageInteractVolume::AFoliageInteractVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	FoliageInteractionBound = CreateDefaultSubobject<UBoxComponent>(TEXT("FoliageInteractionBound"));

}

void AFoliageInteractVolume::BeginPlay()
{
	Super::BeginPlay();
	if (MI_FoliageInteractRenderTargetGenerator != nullptr)
	{
		MID_FoliageInteractRenderTargetGenerator = UMaterialInstanceDynamic::Create(MI_FoliageInteractRenderTargetGenerator, this);
		MID_FoliageInteractRenderTargetGenerator->SetTextureParameterValue("ParameterValue", RT_FoliageInteractImpact);
	}
	if (RT_FoliageInteractImpact != nullptr)
	{
		RT_FoliageInteractImpact->RenderTargetFormat = RTF_RGBA16f;
		RT_FoliageInteractImpact->bAutoGenerateMips = false;
		RT_FoliageInteractImpact->InitAutoFormat(FoliageInteractionBound->Bounds.GetBox().GetSize().X * 2, FoliageInteractionBound->Bounds.GetBox().GetSize().Y * 2);
		RT_FoliageInteractImpact->UpdateResourceImmediate(true);
	}
}

void AFoliageInteractVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (InteractingSourceComponentQueue.IsEmpty()) return;
	TQueue<TScriptInterface<IICharacterMovement>> tempQueue;
	for (int8 i = 0; i < 4; i++)
	{
		//if (InteractingSourceComponentQueue.IsEmpty()) continue;
		TScriptInterface<IICharacterMovement> SourceComponent;
		InteractingSourceComponentQueue.Dequeue(SourceComponent);
		//if (SourceComponent.GetObject() == nullptr || !IsInside(SourceComponent->_GetLocation())) continue;
		SetImpulse(SourceComponent, i);
		if (SourceComponent.GetObject() == nullptr) continue;
		if (IsInside(SourceComponent->_GetLocation()))
		{
			tempQueue.Enqueue(SourceComponent); // Inside Volume : Push Again
		}
		else
		{
			InteractingSourceComponentSet.Remove(SourceComponent);	// Outside Volume : Remove From Set
		}
	}
	for (int8 i = 0; i < 4; i++)
	{
		if (tempQueue.IsEmpty()) break;
		TScriptInterface<IICharacterMovement> SourceComponent;
		tempQueue.Dequeue(SourceComponent);
		if (SourceComponent.GetObject() == nullptr) continue;
		InteractingSourceComponentQueue.Enqueue(SourceComponent);
	}
	if (RT_FoliageInteractImpact != nullptr && MID_FoliageInteractRenderTargetGenerator != nullptr)
	{
		UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT_FoliageInteractImpact, MID_FoliageInteractRenderTargetGenerator);
	}
}

bool AFoliageInteractVolume::IsInside(const FVector& Location)
{
	if (FoliageInteractionBound == nullptr) return false;
	return FoliageInteractionBound->Bounds.GetBox().IsInside(Location);
}

void AFoliageInteractVolume::SetImpulse(const FVector& ImpactLocation, float Radius)
{
	if (RT_FoliageInteractImpact == nullptr || MID_FoliageInteractRenderTargetGenerator == nullptr) return;
	FBox Bounds = FoliageInteractionBound->Bounds.GetBox();
	FVector RelativeLocation = ImpactLocation;
	RelativeLocation.X = (RelativeLocation.X - Bounds.Min.X) / Bounds.GetSize().X;
	RelativeLocation.Y = (RelativeLocation.Y - Bounds.Min.Y) / Bounds.GetSize().Y;
	RelativeLocation.Z = (RelativeLocation.Z - Bounds.Min.Z) / Bounds.GetSize().Z;

	//UE_LOG(LogTemp, Log, TEXT("Scaled Location : %s"), *RelativeLocation.ToString());
	FLinearColor Color{ (float)RelativeLocation.X, (float)RelativeLocation.Y, (float)RelativeLocation.Z, 0.f };

	MID_FoliageInteractRenderTargetGenerator->SetScalarParameterValue(TEXT("Radius"), Radius / FoliageInteractionBound->Bounds.GetBox().GetSize().X);
	MID_FoliageInteractRenderTargetGenerator->SetVectorParameterValue(TEXT("CenterPosition"), Color);
	//UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_FoliageInteractImpact);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT_FoliageInteractImpact, MID_FoliageInteractRenderTargetGenerator);
}

void AFoliageInteractVolume::PushInteractSourceComponent(TScriptInterface<IICharacterMovement>& InSourceComponent)
{
	if (InSourceComponent.GetObject() == nullptr || InteractingSourceComponentSet.Contains(InSourceComponent)) return;
	InteractingSourceComponentSet.Add(InSourceComponent);
	InteractingSourceComponentQueue.Enqueue(InSourceComponent);
}

void AFoliageInteractVolume::SetImpulse(TScriptInterface<IICharacterMovement>& InSourceComponent, int8 SlotIndex)
{
	if (RT_FoliageInteractImpact == nullptr || MID_FoliageInteractRenderTargetGenerator == nullptr) return;

	const FString RadiusParamName = FString::Printf(TEXT("Radius_%d"), SlotIndex);
	const FString CenterPosParamName = FString::Printf(TEXT("CenterPosition_%d"), SlotIndex);
	const FString PowerParamName = FString::Printf(TEXT("Power_%d"), SlotIndex);

	float Radius = 0.f;
	FLinearColor CenterPosition{0.f, 0.f, 0.f, 0.f};
	float Power = 0.f;

	if (InSourceComponent.GetObject() != nullptr && InSourceComponent.GetInterface() != nullptr)
	{
		FBox Bounds = FoliageInteractionBound->Bounds.GetBox();
		FVector RelativeLocation = InSourceComponent->_GetLocation();
		CenterPosition.R = (float)((RelativeLocation.X - Bounds.Min.X) / Bounds.GetSize().X);
		CenterPosition.G = (float)((RelativeLocation.Y - Bounds.Min.Y) / Bounds.GetSize().Y);
		CenterPosition.B = (float)((RelativeLocation.Z - Bounds.Min.Z) / Bounds.GetSize().Z);
		//FLinearColor Color{ (float)RelativeLocation.X, (float)RelativeLocation.Y, (float)RelativeLocation.Z, 0.f };
		Radius = InSourceComponent->GetComponentCollideRadius() / FoliageInteractionBound->Bounds.GetBox().GetSize().X;
		Power = FMath::Max(InSourceComponent->_GetVelocity().Size() / 100.f, 1.f);
	}

	MID_FoliageInteractRenderTargetGenerator->SetScalarParameterValue(FName(*RadiusParamName), Radius);
	MID_FoliageInteractRenderTargetGenerator->SetVectorParameterValue(FName(*CenterPosParamName), CenterPosition);
	MID_FoliageInteractRenderTargetGenerator->SetScalarParameterValue(FName(*PowerParamName), Power);

	if (GEngine)
	{
		const FString DebugMessage = FString::Printf(TEXT("Slot %d: Radius=%.2f, Center=(%.2f, %.2f, %.2f), Power=%.1f"),
			SlotIndex,
			Radius,
			CenterPosition.R, CenterPosition.G, CenterPosition.B,
			Power);
		GEngine->AddOnScreenDebugMessage(SlotIndex, 0.0f, FColor::White, DebugMessage);
	}
}
