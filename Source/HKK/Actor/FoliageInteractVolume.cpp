#include "Actor/FoliageInteractVolume.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInstance.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
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

	UE_LOG(LogTemp, Log, TEXT("Scaled Location : %s"), *RelativeLocation.ToString());
	FLinearColor Color{ (float)RelativeLocation.X, (float)RelativeLocation.Y, (float)RelativeLocation.Z, 0.f };

	MID_FoliageInteractRenderTargetGenerator->SetScalarParameterValue(TEXT("Radius"), Radius / FoliageInteractionBound->Bounds.GetBox().GetSize().X);
	MID_FoliageInteractRenderTargetGenerator->SetVectorParameterValue(TEXT("CenterPosition"), Color);
	UKismetRenderingLibrary::ClearRenderTarget2D(this, RT_FoliageInteractImpact);
	UKismetRenderingLibrary::DrawMaterialToRenderTarget(this, RT_FoliageInteractImpact, MID_FoliageInteractRenderTargetGenerator);
}

