// Fill out your copyright notice in the Description page of Project Settings.

#include "PlantBase.h"
#include <Components/InstancedStaticMeshComponent.h>


void APlantBase::SetMesh(UStaticMesh* PlantMesh, UMaterialInstance* PlantMaterial)
{
	InstancedStaticMeshComponent->SetStaticMesh(PlantMesh);
	InstancedStaticMeshComponent->SetMaterial(0, PlantMaterial);
}

void APlantBase::AddMesh(FTransform Transform, FIntVector Coord)
{
	check(!InstanceMap.Contains(Coord));
	int32 Index = InstancedStaticMeshComponent->AddInstanceWorldSpace(Transform);
	InstanceMap.Add(Coord, Index);
}

void APlantBase::RemoveMeshIfExists(FIntVector Coord)
{
	if (!InstanceMap.Contains(Coord))
		return;

	int32 Index = InstanceMap[Coord];
	check(InstancedStaticMeshComponent->RemoveInstance(Index));

	for (auto & Pair : InstanceMap)
	{
		if (Pair.Value <= Index)
			continue;

		InstanceMap[Pair.Key]--;
	}

	InstanceMap.Remove(Coord);
}

// Sets default values
APlantBase::APlantBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("PlantMesh"));
	InstancedStaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = InstancedStaticMeshComponent;
}