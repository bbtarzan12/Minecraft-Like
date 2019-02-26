// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CubicWorldCharacter.generated.h"

class UInputComponent;

UCLASS()
class CUBICWORLD_API ACubicWorldCharacter : public ACharacter
{
	GENERATED_BODY()

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

public:
	// Sets default values for this character's properties
	ACubicWorldCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void PlaceVoxel();
	void DeleteVoxel();

	/** Handles moving forward/backward */
	void MoveForward(float Value);

	/** Handles stafing movement, left and right */
	void MoveRight(float Value);

};
