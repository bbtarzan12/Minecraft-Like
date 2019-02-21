// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Plant.generated.h"

enum class EVoxelMaterial : uint8;
enum class EVoxelType : uint8;

UCLASS()
class MINECRAFT_API APlant : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UProceduralMeshComponent* ProceduralMeshComponent;
	
public:	
	// Sets default values for this actor's properties
	APlant();

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	void GeneratePlantMesh(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial, const EVoxelType& VoxelType);

};
