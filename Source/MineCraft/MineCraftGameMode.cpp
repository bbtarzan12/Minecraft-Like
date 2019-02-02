// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MineCraftGameMode.h"
#include "MineCraftHUD.h"
#include "MineCraftCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Public/MinecraftGameState.h"

AMineCraftGameMode::AMineCraftGameMode() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;


	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMineCraftHUD::StaticClass();

	GameStateClass = AMinecraftGameState::StaticClass();
}