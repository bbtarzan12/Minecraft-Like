// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "SimplexNoiseBPLibrary.h"
#include "MineCraft.h"
#include "DrawDebugHelpers.h"
#include "Async/ParallelFor.h"
#include "Public/ChunkMeshTask.h"
#include "Public/ChunkUtil.h"
#include "Public/Plant.h"

TMap<EVoxelMaterial, UMaterialInstanceDynamic*> AChunk::VoxelMaterials;

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	if (VoxelMaterials.Num() == 0)
	{
		const UEnum* VoxelTypePtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EVoxelMaterial"), true);
		if (VoxelTypePtr != nullptr)
		{
			for (int i = 1; i < VoxelTypePtr->NumEnums() - 1; i++)
			{
				EVoxelMaterial VoxelType = (EVoxelMaterial) (VoxelTypePtr->GetValueByIndex(i));
				FName VoxelEnumName = VoxelTypePtr->GetNameByIndex(i);
				FString VoxelMaterialName = VoxelEnumName.ToString();
				VoxelMaterialName.RemoveFromStart("EVoxelMaterial::");

				FString MaterialPath = "MaterialInstanceConstant'/Game/Materials/Voxel/";
				MaterialPath.Append(VoxelMaterialName);
				MaterialPath.AppendChar('.');
				MaterialPath.Append(VoxelMaterialName);
				MaterialPath.AppendChar('\'');
				ConstructorHelpers::FObjectFinder<UMaterialInterface> FoundMaterial(*MaterialPath);
				if (FoundMaterial.Succeeded())
				{
					UE_LOG(LogChunk, Log, TEXT("Load Voxel Material : %s"), *FoundMaterial.Object->GetFName().ToString());
					VoxelMaterials.Add(VoxelType, (UMaterialInstanceDynamic*) FoundMaterial.Object);
				}
			}
		}
	}
}

void AChunk::StartTask()
{
	(new FAsyncTask<ChunkMeshTask>(this, ChunkOffset, ChunkSize, VoxelSize, NoiseWeight, NoiseScale, RandomSeed))->StartBackgroundTask();
}


void AChunk::StartTask(int32 Index, EVoxelMaterial VoxelMaterial, EVoxelType VoxelType)
{
	(new FAsyncTask<ChunkMeshTask>(this, ChunkSize, VoxelSize, VoxelData, Index, VoxelMaterial, VoxelType))->StartSynchronousTask();
}

void AChunk::GeneratePlantMesh(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial, const EVoxelType& VoxelType)
{
	APlant* Plant = GetWorld()->SpawnActor<APlant>(APlant::StaticClass());
	Plant->GeneratePlantMesh(GlobalLocation, VoxelMaterial, VoxelType);
	PlantData.Add(Plant);
}

void AChunk::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ProceduralMeshComponent = NewObject<UProceduralMeshComponent>(this, FName("ProceduralMeshComponent"));
	ProceduralMeshComponent->RegisterComponent();

	RootComponent = ProceduralMeshComponent;
	RootComponent->SetWorldTransform(Transform);
}

void AChunk::Init(int32 RandomSeed, FIntVector ChunkSize, float NoiseScale, float NoiseWeight, int32 VoxelSize)
{
	this->RandomSeed = RandomSeed;
	this->ChunkSize = ChunkSize;
	this->NoiseScale = NoiseScale;
	this->NoiseWeight = NoiseWeight;
	this->VoxelSize = VoxelSize;

	Location = GetActorLocation();
	ChunkOffset = FIntVector(Location);
	ChunkOffset.X /= (ChunkSize.X * VoxelSize);
	ChunkOffset.Y /= (ChunkSize.Y * VoxelSize);
	ChunkOffset.Z /= (ChunkSize.Z * VoxelSize);

	VoxelSizeHalf = VoxelSize / 2;
	StartTask();
}

void AChunk::GenerateMesh(const TMap<EVoxelMaterial, FChunkMesh*>& MeshData)
{
	ProceduralMeshComponent->ClearAllMeshSections();
	for (auto & Pair : MeshData)
	{
		int32 SectionIndex = (int32)Pair.Key - 1;
		FChunkMesh* ChunkMesh = Pair.Value;
		bool bCollider = Pair.Key != EVoxelMaterial::TALLGRASS;

		if (ChunkMesh)
		{
			ProceduralMeshComponent->CreateMeshSection(SectionIndex, ChunkMesh->Vertices, ChunkMesh->Triangles, ChunkMesh->Normals, ChunkMesh->UVs, ChunkMesh->VertexColors, ChunkMesh->Tangents, bCollider);
			ProceduralMeshComponent->SetMaterial(SectionIndex, VoxelMaterials[Pair.Key]);
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Chunk Mesh Generated: %s"), *GetFName().ToString()));
}

void AChunk::SetVoxelData(const TArray<FVoxelFace>& VoxelData)
{
	this->VoxelData = VoxelData;
}

void AChunk::SetVoxel(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial, const EVoxelType& VoxelType)
{
	FIntVector LocalVoxelLocation = ChunkUtil::ConvertGlobalToLocal(GlobalLocation, Location, VoxelSize);
	int32 Index = ChunkUtil::Convert3Dto1DIndex(LocalVoxelLocation, ChunkSize);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Global Location: %s"), *GlobalLocation.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Local Location: %s"), *LocalVoxelLocation.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Index: %d"), Index));

	switch (VoxelType)
	{
		case EVoxelType::Voxel:
			StartTask(Index, VoxelMaterial, VoxelType);
			break;
		case EVoxelType::PlantMesh:
			GeneratePlantMesh(GlobalLocation, VoxelMaterial, VoxelType);
			break;
		case EVoxelType::NONE:
		default:
			break;
	}
}