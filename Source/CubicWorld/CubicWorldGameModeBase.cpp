// Fill out your copyright notice in the Description page of Project Settings.

#include "CubicWorldGameModeBase.h"
#include <ConstructorHelpers.h>
#include "CubicWorldHud.h"
#include "CubicWorldGameState.h"

ACubicWorldGameModeBase::ACubicWorldGameModeBase()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/CubicWorldCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ACubicWorldHud::StaticClass();
	GameStateClass = ACubicWorldGameState::StaticClass();

}
