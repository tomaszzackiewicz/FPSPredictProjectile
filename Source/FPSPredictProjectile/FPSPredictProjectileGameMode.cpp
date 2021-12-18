// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSPredictProjectileGameMode.h"
#include "FPSPredictProjectileHUD.h"
#include "FPSPredictProjectileCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPSPredictProjectileGameMode::AFPSPredictProjectileGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSPredictProjectileHUD::StaticClass();
}
