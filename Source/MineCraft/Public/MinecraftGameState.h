// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MinecraftGameState.generated.h"

/**
 * 
 */
UCLASS()
class MINECRAFT_API AMinecraftGameState : public AGameStateBase
{
	GENERATED_BODY()
	
	AMinecraftGameState();

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
	int32 ChunkIteration = 4;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void CheckPlayerChunkLocation();
	void ProcessChunkQueue();
	void SetChunkToLoad(const FIntVector& ActorChunkLocation);
	bool IsChunkIsAlreadyVisited(const FIntVector& ChunkLocation);

	UPROPERTY()
	TArray<FIntVector> PreviousActorChunkLocationArray;

	TSet<FIntVector> VisitedChunkLocation;
	TQueue<FVector> ChunkQueue;
	
};
