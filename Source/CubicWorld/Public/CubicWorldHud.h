// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CubicWorldHud.generated.h"

/**
 * 
 */
UCLASS()
class CUBICWORLD_API ACubicWorldHud : public AHUD
{
	GENERATED_BODY()
	ACubicWorldHud();
	
public:
	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
};
