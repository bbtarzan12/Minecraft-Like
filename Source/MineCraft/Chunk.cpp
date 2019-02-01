// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "SimplexNoiseBPLibrary.h"
#include "MineCraft.h"
#include "DrawDebugHelpers.h"
#include "ChunkMeshGenerator.h"
#include "Async/ParallelFor.h"

TArray<UMaterialInstanceDynamic*> AChunk::VoxelMaterials;

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (VoxelMaterials.Num() == 0)
	{
		const UEnum* VoxelTypePtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EVoxelType"), true);
		if (VoxelTypePtr != nullptr)
		{
			for (int i = 0; i < VoxelTypePtr->NumEnums() - 1; i++)
			{
				EVoxelType VoxelType = (EVoxelType) (VoxelTypePtr->GetValueByIndex(i));
				FName VoxelEnumName = VoxelTypePtr->GetNameByIndex(i);
				FString VoxelMaterialName = VoxelEnumName.ToString();
				VoxelMaterialName.RemoveFromStart("EVoxelType::");

				FString MaterialPath = "MaterialInstanceConstant'/Game/Materials/Voxel/";
				MaterialPath.Append(VoxelMaterialName);
				MaterialPath.AppendChar('.');
				MaterialPath.Append(VoxelMaterialName);
				MaterialPath.AppendChar('\'');
				ConstructorHelpers::FObjectFinder<UMaterialInterface> FoundMaterial(*MaterialPath);
				if (FoundMaterial.Succeeded())
				{
					UE_LOG(LogChunk, Log, TEXT("Load Voxel Material : %s"), *FoundMaterial.Object->GetFName().ToString());
					VoxelMaterials.Add((UMaterialInstanceDynamic*) FoundMaterial.Object);
				}
			}
		}
	}
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
	for (auto & Pair : Worker->MeshData)
	{
		int32 SectionIndex = (int32)Pair.Key;
		FChunkMesh* ChunkMesh = Pair.Value;
		ProceduralMeshComponent->CreateMeshSection(SectionIndex, ChunkMesh->Vertices, ChunkMesh->Triangles, ChunkMesh->Normals, ChunkMesh->UVs, ChunkMesh->VertexColors, ChunkMesh->Tangents, true);
		ProceduralMeshComponent->SetMaterial(SectionIndex, VoxelMaterials[SectionIndex]);
	}

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