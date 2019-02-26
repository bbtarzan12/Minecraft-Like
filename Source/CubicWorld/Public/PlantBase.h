// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlantBase.generated.h"

class UInstancedStaticMeshComponent;

UCLASS()
class CUBICWORLD_API APlantBase : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UInstancedStaticMeshComponent* InstancedStaticMeshComponent;
	
public:
	// Sets default values for this actor's properties
	APlantBase();

	void SetMesh(UStaticMesh* PlantMesh, UMaterialInstance* PlantMaterial);
	void AddMesh(FTransform Transform, FIntVector Coord);
	void RemoveMeshIfExists(FIntVector Coord);

private:
	TMap<FIntVector, int32> InstanceMap;

};
