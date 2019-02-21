// Fill out your copyright notice in the Description page of Project Settings.

#include "MinecraftGameState.h"
#include "Chunk.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Public/ChunkUtil.h"

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

	for (int X = ActorChunkLocation.X - ChunkIteration.X; X <= ActorChunkLocation.X + ChunkIteration.X; X++)
	{
		for (int Y = ActorChunkLocation.Y - ChunkIteration.Y; Y <= ActorChunkLocation.Y + ChunkIteration.Y; Y++)
		{
			for (int Z = ActorChunkLocation.Z - ChunkIteration.Z; Z <= ActorChunkLocation.Z + ChunkIteration.Z; Z++)
			{
				const FIntVector CurrentChunkLocation = FIntVector(X * ChunkSize.X, Y * ChunkSize.Y, Z * ChunkSize.Z) * VoxelSize;

				if (VisitedChunkMap.Contains(CurrentChunkLocation))
				{
					continue;
				}

				ChunkQueue.Add(FVector(CurrentChunkLocation));
				VisitedChunkMap.Add(CurrentChunkLocation, nullptr);
			}
		}
	}
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

void AMinecraftGameState::SetVoxel(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial, const EVoxelType& VoxelType)
{
	bool bCheck = false;
	for (auto & Pair : VisitedChunkMap)
	{
		FIntVector ChunkLocation = Pair.Key;
		FIntVector LocalLocation = ChunkUtil::ConvertGlobalToLocal(GlobalLocation, FVector(ChunkLocation), VoxelSize);
		
		if (ChunkUtil::BoundaryCheck3D(LocalLocation, ChunkSize))
		{
			check(bCheck == false);
			AChunk* Chunk = Pair.Value;
			Chunk->SetVoxel(GlobalLocation, VoxelMaterial, VoxelType);
			bCheck = true;
		}
	}
	check(bCheck == true);
}

void AMinecraftGameState::SetVoxel(const FVector& GlobalLocation, const FVector& Normal, const EVoxelMaterial& VoxelMaterial, const EVoxelType& VoxelType)
{
	SetVoxel(GlobalLocation + Normal * 0.01f, VoxelMaterial, VoxelType);
}

void AMinecraftGameState::DeleteVoxel(const FVector& GlobalLocation, const FVector& Forward)
{
	SetVoxel(GlobalLocation + Forward * 0.01f, EVoxelMaterial::NONE, EVoxelType::NONE);
}

void AMinecraftGameState::CheckPlayerChunkLocation()
{
	for (int32 PlayerIdx = 0; PlayerIdx < PlayerArray.Num(); PlayerIdx++)
	{
		FVector ActorLoaction = UGameplayStatics::GetPlayerPawn(GetWorld(), PlayerIdx)->GetActorLocation();
		FIntVector ActorChunkLocation = ChunkUtil::ConvertLocationToOffset(ActorLoaction, ChunkSize, VoxelSize);

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
	if (ChunkQueue.Num() == 0)
		return;

	const FRotator ChunkRotation = FRotator::ZeroRotator;

	// 임시로 플레이어 0번째를 가져다 씀
	FVector ActorLoaction = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	Algo::Sort(ChunkQueue, [ActorLoaction](const FVector& A, const FVector& B)
	{
		float DistanceA = FVector::DistSquared(ActorLoaction, A);
		float DistanceB = FVector::DistSquared(ActorLoaction, B);

		return DistanceA < DistanceB;
	});

	FVector& ChunkLocation = ChunkQueue[0];

	AChunk* Chunk = GetWorld()->SpawnActor<AChunk>(ChunkLocation, ChunkRotation);
	Chunk->Init(RandomSeed, ChunkSize, NoiseScale, NoiseWeight, VoxelSize);

	check(VisitedChunkMap.Contains(FIntVector(ChunkLocation)));
	VisitedChunkMap[FIntVector(ChunkLocation)] = Chunk;

	ChunkQueue.RemoveSingle(ChunkLocation);
}
