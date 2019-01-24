// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "WorldGenerator.generated.h"

UCLASS()
class MINECRAFT_API AWorldGenerator : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	int32 randomSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FIntVector ChunkSize = FIntVector(32, 32, 32);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	float NoiseScale = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	float NoiseThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	int32 VoxelSize = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FIntVector ChunkLocation;

	UPROPERTY()
	int32 VoxelSizeHalf;

	UPROPERTY()
	TArray <int32> ChunkFields;


public:
	// Sets default values for this actor's properties
	AWorldGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void GenerateChunk();
	virtual void UpdateMesh();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
