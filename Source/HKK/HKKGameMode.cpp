// Copyright Epic Games, Inc. All Rights Reserved.

#include "HKKGameMode.h"
#include "HKKPlayerController.h"
#include "HKKCharacter.h"
#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHKKGameMode::AHKKGameMode()
{
	PlayerControllerClass = AHKKPlayerController::StaticClass();
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/_Player/Blueprint/BP_Trooper"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}