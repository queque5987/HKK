#include "GameFramework/GameMode_Stage.h"
#include "HKKPlayerController.h"
#include "PlayerCharacter.h"
#include "GameFramework/HKK_PlayerState.h"
#include "UObject/ConstructorHelpers.h"

AGameMode_Stage::AGameMode_Stage() : Super()
{
	PlayerControllerClass = AHKKPlayerController::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/_Player/Blueprint/BP_Robo"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if (PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

	PlayerStateClass = AHKK_PlayerState::StaticClass();
}
