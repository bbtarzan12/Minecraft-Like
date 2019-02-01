// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VoxelGenerator.generated.h"

class AChunk;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MINECRAFT_API UVoxelGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVoxelGenerator();

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
	int32 ChunkIteration = 2;

	TSet<FIntVector> VisitedChunkLocation;

private:
	FIntVector PreviousChunkLocation = FIntVector::NoneValue;
	FIntVector ActorChunkLocation = FIntVector::ZeroValue;
	FTimerHandle ChunkTimerHandle;
	TQueue<FVector> ChunkQueue;

	AChunk* CurrentChunk;

	void GenerateChunks();
	void ProcessChunkQueue();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
