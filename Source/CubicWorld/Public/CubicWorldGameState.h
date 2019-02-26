// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CubicWorldGameState.generated.h"

class AChunk;
enum class EVoxelMaterial : uint8;

UCLASS()
class CUBICWORLD_API ACubicWorldGameState : public AGameStateBase
{
	GENERATED_BODY()
	ACubicWorldGameState();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	int32 RandomSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FIntVector ChunkSize = FIntVector(32, 32, 32);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	float NoiseScale = 0.015f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	float NoiseWeight = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	int32 VoxelSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FIntVector ChunkIteration = FIntVector(5, 5, 2);

public:
	void SetVoxel(const FVector& GlobalLocation, const FVector& Normal, const EVoxelMaterial& VoxelMaterial);
	void DeleteVoxel(const FVector& GlobalLocation, const FVector& Forward);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void SetVoxel(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial);
	void CheckPlayerChunkLocation();
	void ProcessChunkQueue();
	void SetChunkToLoad(const FIntVector& ActorChunkLocation);

	UPROPERTY()
	TArray<FIntVector> PreviousActorChunkLocationArray;

	TMap<FIntVector, AChunk*> VisitedChunkMap;
	TQueue<FVector> OldChunkQueue;
	TArray<FVector> ChunkQueue;
};
