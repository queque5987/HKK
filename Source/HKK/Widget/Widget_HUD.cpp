#include "Widget/Widget_HUD.h"

void UWidget_HUD::OnUpdatePlayerStatFloat_Implementation(const EPlayerStatType PlayerStatType, float CurrStat, float MaxStat)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("UI Function Called! HP: %f / %f"), CurrStat, MaxStat));
	switch (PlayerStatType)
	{
	case(EPlayerStatType::EPST_HP):
		if (HPBar == nullptr) return;
		HPBar->SetPercent(CurrStat / MaxStat);
		break;
	default:
		break;
	}

}
