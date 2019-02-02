// Fill out your copyright notice in the Description page of Project Settings.

#include "MinecraftGameState.h"
#include "Chunk.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

AMinecraftGameState::AMinecraftGameState() : Super()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AMinecraftGameState::SetChunkToLoad(const FIntVector& ActorChunkLocation)
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

				if (IsChunkIsAlreadyVisited(CurrentChunkLocation))
				{
					continue;
				}

				ChunkQueue.Enqueue(FVector(CurrentChunkLocation));
				VisitedChunkLocation.Add(CurrentChunkLocation);
			}
		}
	}
}

bool AMinecraftGameState::IsChunkIsAlreadyVisited(const FIntVector& ChunkLocation)
{
	return VisitedChunkLocation.Contains(ChunkLocation);
}

void AMinecraftGameState::BeginPlay()
{
	PreviousActorChunkLocationArray.SetNumUninitialized(PlayerArray.Num());
}

void AMinecraftGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CheckPlayerChunkLocation();
	ProcessChunkQueue();
}

void AMinecraftGameState::CheckPlayerChunkLocation()
{
	for (int32 PlayerIdx = 0; PlayerIdx < PlayerArray.Num(); PlayerIdx++)
	{
		FVector ActorLoaction = UGameplayStatics::GetPlayerPawn(GetWorld(), PlayerIdx)->GetActorLocation();
		
		ActorLoaction.X = FMath::FloorToInt(ActorLoaction.X / VoxelSize / ChunkSize.X);
		ActorLoaction.Y = FMath::FloorToInt(ActorLoaction.Y / VoxelSize / ChunkSize.Y);
		ActorLoaction.Z = FMath::FloorToInt(ActorLoaction.Z / VoxelSize / ChunkSize.Z);

		FIntVector ActorChunkLocation = FIntVector(ActorLoaction);

		if (PreviousActorChunkLocationArray[PlayerIdx] == ActorChunkLocation)
		{
			continue;
		}

		SetChunkToLoad(ActorChunkLocation);
		PreviousActorChunkLocationArray[PlayerIdx] = ActorChunkLocation;
	}
}

void AMinecraftGameState::ProcessChunkQueue()
{
	if (ChunkQueue.IsEmpty())
		return;

	const FRotator ChunkRotation = FRotator::ZeroRotator;

	FVector ChunkLocation;
	ChunkQueue.Dequeue(ChunkLocation);

	AChunk* Chunk = GetWorld()->SpawnActor<AChunk>(FVector(ChunkLocation), ChunkRotation);
	Chunk->Init(RandomSeed, ChunkSize, NoiseScale, NoiseWeight, VoxelSize);
}
