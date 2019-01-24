// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MineCraftGameMode.h"
#include "MineCraftHUD.h"
#include "MineCraftCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMineCraftGameMode::AMineCraftGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMineCraftHUD::StaticClass();
}
