// Fill out your copyright notice in the Description page of Project Settings.

#include "Plant.h"
#include "Chunk.h"

// Sets default values
APlant::APlant()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

}

void APlant::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ProceduralMeshComponent = NewObject<UProceduralMeshComponent>(this, FName("ProceduralMeshComponent"));
	ProceduralMeshComponent->RegisterComponent();

	RootComponent = ProceduralMeshComponent;
	RootComponent->SetWorldTransform(Transform);
}

void APlant::GeneratePlantMesh(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial, const EVoxelType& VoxelType)
{

}
