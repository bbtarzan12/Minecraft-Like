// Fill out your copyright notice in the Description page of Project Settings.

#include "VoxelGenerator.h"
#include "MineCraft.h"
#include "Chunk.h"

// Sets default values for this component's properties
UVoxelGenerator::UVoxelGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVoxelGenerator::BeginPlay()
{
	Super::BeginPlay();

	//GetWorld()->GetTimerManager().SetTimer(ChunkTimerHandle, this, &UVoxelGenerator::ProcessChunkQueue, 0.01f, true);
}


void UVoxelGenerator::GenerateChunks()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Yellow, FString::Printf(TEXT("GenerateChunks Actor Location : %d %d %d"), ActorChunkLocation.X, ActorChunkLocation.Y, ActorChunkLocation.Z));
	}

	for (int X = ActorChunkLocation.X - ChunkIteration; X <= ActorChunkLocation.X + ChunkIteration; X++)
	{
		for (int Y = ActorChunkLocation.Y - ChunkIteration; Y <= ActorChunkLocation.Y + ChunkIteration; Y++)
		{
			for (int Z = ActorChunkLocation.Z - ChunkIteration; Z <= ActorChunkLocation.Z + ChunkIteration; Z++)
			{
				const FIntVector CurrentChunkLocation = FIntVector(X * ChunkSize.X, Y * ChunkSize.Y, Z * ChunkSize.Z) * VoxelSize;

				if (VisitedChunkLocation.Contains(CurrentChunkLocation))
				{
					continue;
				}

				ChunkQueue.Enqueue(FVector(CurrentChunkLocation));
				VisitedChunkLocation.Add(CurrentChunkLocation);
			}
		}
	}
}

void UVoxelGenerator::ProcessChunkQueue()
{
	if (ChunkQueue.IsEmpty())
		return;

	const FRotator ChunkRotation = FRotator::ZeroRotator;

	FVector ChunkLocation;
	ChunkQueue.Dequeue(ChunkLocation);

	AChunk* Chunk = GetWorld()->SpawnActor<AChunk>(FVector(ChunkLocation), ChunkRotation);
	Chunk->Init(RandomSeed, ChunkSize, NoiseScale, NoiseWeight, VoxelSize);
}

// Called every frame
void UVoxelGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ProcessChunkQueue();

	FVector ActorLoaction = GetOwner()->GetActorLocation();
	ActorChunkLocation.X = FMath::FloorToInt(ActorLoaction.X / VoxelSize / ChunkSize.X);
	ActorChunkLocation.Y = FMath::FloorToInt(ActorLoaction.Y / VoxelSize / ChunkSize.Y);
	ActorChunkLocation.Z = FMath::FloorToInt(ActorLoaction.Z / VoxelSize / ChunkSize.Z);

	if (ActorChunkLocation == PreviousChunkLocation)
		return;

	GenerateChunks();

	PreviousChunkLocation = ActorChunkLocation;
}

