// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "SimplexNoiseBPLibrary.h"
#include "MineCraft.h"
#include "DrawDebugHelpers.h"
#include "ChunkMeshGenerator.h"
#include "Async/ParallelFor.h"

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AChunk::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ProceduralMeshComponent = NewObject<UProceduralMeshComponent>(this, FName("ProceduralMeshComponent"));
	ProceduralMeshComponent->RegisterComponent();

	RootComponent = ProceduralMeshComponent;
	RootComponent->SetWorldTransform(Transform);
}

void AChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Worker == nullptr || !Worker->IsFinished())
	{
		return;
	}

	ProceduralMeshComponent->ClearAllMeshSections();
	ProceduralMeshComponent->CreateMeshSection(0, Worker->Vertices, Worker->Triangles, Worker->Normals, Worker->UVs, Worker->VertexColors, Worker->Tangents, true);

	SetActorTickEnabled(false);
	delete Worker;
}

void AChunk::Init(int32 RandomSeed, FIntVector ChunkSize, float NoiseScale, float NoiseWeight, int32 VoxelSize)
{
	this->RandomSeed = RandomSeed;
	this->ChunkSize = ChunkSize;
	this->NoiseScale = NoiseScale;
	this->NoiseWeight = NoiseWeight;
	this->VoxelSize = VoxelSize;

	FVector Location = GetActorLocation();
	ChunkLocation = FIntVector(Location);
	ChunkLocation.X /= (ChunkSize.X * VoxelSize);
	ChunkLocation.Y /= (ChunkSize.Y * VoxelSize);
	ChunkLocation.Z /= (ChunkSize.Z * VoxelSize);

	VoxelSizeHalf = VoxelSize / 2;

	Worker = new FChunkMeshGenerator(ChunkLocation, ChunkSize, VoxelSize, NoiseWeight, NoiseScale, RandomSeed);
}