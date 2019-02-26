// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelCommon.h"
#include <AsyncWork.h>
#include <Components/StaticMeshComponent.h>
#include "PlantBase.h"
#include "Chunk.generated.h"

struct FChunkMesh;
class UDataTable;
class ChunkMeshTask;
class UInstancedStaticMeshComponent;
class UCubicWorldInstance;
enum class EVoxelMaterial : uint8;

UCLASS()
class CUBICWORLD_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChunk();
	~AChunk();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Chunk)
	TMap<EVoxelMaterial, APlantBase*> PlantActors;

	UPROPERTY(EditAnywhere)
	TArray<FVoxel> VoxelData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	int32 RandomSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FIntVector ChunkSize = FIntVector(32, 32, 32);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	float NoiseScale = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	float NoiseWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	int32 VoxelSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FIntVector ChunkOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FVector Location;

	UPROPERTY()
	int32 VoxelSizeHalf;

private:
	FAsyncTask<ChunkMeshTask>* ChunkTask;
	bool bWaitingForTask;
	bool bIsFirstTime;
	static UCubicWorldInstance* WorldInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	void StartTask();
	void StartTask(int32 Index, EVoxelMaterial VoxelMaterial);

public:
	virtual void Tick(float DeltaSeconds) override;
	void SetVoxel(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial);
	void Init(int32 RandomSeed, FIntVector ChunkSize, float NoiseScale, float NoiseWeight, int32 VoxelSize);
	void GenerateMesh(const TMap<EVoxelMaterial, FChunkMesh*>& MeshData);
	void SetVoxelData(const TArray<FVoxel>& VoxelData);
	void SetPlantData(const TMap<EVoxelMaterial, TArray<FIntVector>>& PlantData);

private:
	static UCubicWorldInstance* GetWorldInstance();

};
