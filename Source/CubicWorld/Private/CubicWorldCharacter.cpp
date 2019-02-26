// Fill out your copyright notice in the Description page of Project Settings.

#include "CubicWorldCharacter.h"
#include <Components/CapsuleComponent.h>
#include <Components/InputComponent.h>
#include <Camera/CameraComponent.h>
#include <DrawDebugHelpers.h>
#include "CubicWorldGameState.h"
#include "VoxelCommon.h"

// Sets default values
ACubicWorldCharacter::ACubicWorldCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-5.0f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

}

// Called when the game starts or when spawned
void ACubicWorldCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called to bind functionality to input
void ACubicWorldCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Mouse Left", IE_Pressed, this, &ACubicWorldCharacter::PlaceVoxel);
	PlayerInputComponent->BindAction("Mouse Right", IE_Pressed, this, &ACubicWorldCharacter::DeleteVoxel);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACubicWorldCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACubicWorldCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void ACubicWorldCharacter::PlaceVoxel()
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
			Cast<ACubicWorldGameState>(GetWorld()->GetGameState())->SetVoxel(HitCall.ImpactPoint, HitCall.ImpactNormal, EVoxelMaterial::DIRT);
		}
	}
}

void ACubicWorldCharacter::DeleteVoxel()
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
			Cast<ACubicWorldGameState>(GetWorld()->GetGameState())->DeleteVoxel(HitCall.ImpactPoint, TraceForward);
		}
	}
}

void ACubicWorldCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACubicWorldCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

