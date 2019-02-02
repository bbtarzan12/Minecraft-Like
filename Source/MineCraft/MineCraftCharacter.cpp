// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MineCraftCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "DrawDebugHelpers.h"
#include "Chunk.h"
#include "Public/MinecraftGameState.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMineCraftCharacter

AMineCraftCharacter::AMineCraftCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-5.0f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

void AMineCraftCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMineCraftCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Mouse Left", IE_Pressed, this, &AMineCraftCharacter::PlaceVoxel);
	PlayerInputComponent->BindAction("Mouse Right", IE_Pressed, this, &AMineCraftCharacter::DeleteVoxel);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMineCraftCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMineCraftCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMineCraftCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMineCraftCharacter::LookUpAtRate);
}

void AMineCraftCharacter::PlaceVoxel()
{
	FHitResult HitCall(ForceInit);
	FCollisionQueryParams ParamsCall = FCollisionQueryParams(FName(TEXT("TraceFire")), false, this);

	const FVector TraceLocation = FirstPersonCameraComponent->GetComponentLocation();
	const FVector TraceForward = FirstPersonCameraComponent->GetForwardVector();

	FVector Start = TraceLocation;
	FVector End = TraceLocation + TraceForward * 1000.0f;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	bool Traced = GetWorld()->LineTraceSingleByChannel(HitCall, Start, End, ECC_Visibility, ParamsCall);

	if (Traced)
	{
		if (HitCall.bBlockingHit)
		{
			Cast<AMinecraftGameState>(GetWorld()->GetGameState())->SetVoxel(HitCall.ImpactPoint, HitCall.ImpactNormal, EVoxelType::DIRT);
		}
	}
}

void AMineCraftCharacter::DeleteVoxel()
{
	FHitResult HitCall(ForceInit);
	FCollisionQueryParams ParamsCall = FCollisionQueryParams(FName(TEXT("TraceFire")), false, this);
	
	const FVector TraceLocation = FirstPersonCameraComponent->GetComponentLocation();
	const FVector TraceForward = FirstPersonCameraComponent->GetForwardVector();

	FVector Start = TraceLocation;
	FVector End = TraceLocation + TraceForward * 1000.0f;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	bool Traced = GetWorld()->LineTraceSingleByChannel(HitCall, Start, End, ECC_Visibility, ParamsCall);

	if (Traced)
	{
		if (HitCall.bBlockingHit)
		{
			Cast<AMinecraftGameState>(GetWorld()->GetGameState())->DeleteVoxel(HitCall.ImpactPoint, TraceForward);
		}
	}

}


void AMineCraftCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMineCraftCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMineCraftCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMineCraftCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}