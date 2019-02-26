// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk.h"
#include "ChunkMeshTask.h"
#include "ChunkUtil.h"
#include <ConstructorHelpers.h>
#include <Engine/DataTable.h>
#include "CubicWorld.h"
#include "VoxelData.h"
#include <Components/InstancedStaticMeshComponent.h>
#include "CubicWorldInstance.h"

UCubicWorldInstance* AChunk::WorldInstance;

// Sets default values
AChunk::AChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

AChunk::~AChunk()
{

}

// Called when the game starts or when spawned
void AChunk::BeginPlay()
{
	Super::BeginPlay();

	if (!WorldInstance)
	{
		WorldInstance = Cast<UCubicWorldInstance>(GetGameInstance());
	}
}

void AChunk::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bWaitingForTask)
		return;

	if (!ChunkTask)
		return;

	if (!ChunkTask->IsDone())
		return;

	ChunkMeshTask& Task = ChunkTask->GetTask();
	if (Task.IsTaskFinished())
	{
		SetVoxelData(Task.ResultVoxelData);
		GenerateMesh(Task.MeshData);
		if (bIsFirstTime)
		{
			SetPlantData(Task.PlantData);
		}

		bWaitingForTask = false;
		bIsFirstTime = false;
		PrimaryActorTick.SetTickFunctionEnable(false);
		delete ChunkTask;
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

void AChunk::StartTask()
{
	ChunkTask = new FAsyncTask<ChunkMeshTask>(this, ChunkOffset, ChunkSize, VoxelSize, NoiseWeight, NoiseScale, RandomSeed, GetWorldInstance()->GetRawVoxelDataTable());
	ChunkTask->StartBackgroundTask();
	bWaitingForTask = true;
	bIsFirstTime = true;
	PrimaryActorTick.SetTickFunctionEnable(true);
}

void AChunk::StartTask(int32 Index, EVoxelMaterial VoxelMaterial)
{
	ChunkTask = new FAsyncTask<ChunkMeshTask>(this, ChunkSize, VoxelSize, VoxelData, Index, VoxelMaterial, GetWorldInstance()->GetRawVoxelDataTable());
	ChunkTask->StartSynchronousTask();
	bWaitingForTask = true;
	bIsFirstTime = false;
	PrimaryActorTick.SetTickFunctionEnable(true);
}

UCubicWorldInstance* AChunk::GetWorldInstance()
{
	check(WorldInstance);
	return WorldInstance;
}

void AChunk::SetVoxel(const FVector & GlobalLocation, const EVoxelMaterial & VoxelMaterial)
{
	FIntVector LocalVoxelLocation = ChunkUtil::ConvertGlobalToLocal(GlobalLocation, Location, VoxelSize);
	int32 Index = ChunkUtil::Convert3Dto1DIndex(LocalVoxelLocation, ChunkSize);

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Global Location: %s"), *GlobalLocation.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Local Location: %s"), *LocalVoxelLocation.ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Index: %d"), Index));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Material: %s -> %s"), *GETENUMSTRING("EVoxelMaterial", VoxelData[Index].Material), *GETENUMSTRING("EVoxelMaterial", VoxelMaterial)));
	StartTask(Index, VoxelMaterial);

	if (VoxelMaterial == EVoxelMaterial::NONE)
	{
		for (auto & Pair : PlantActors)
		{
			Pair.Value->RemoveMeshIfExists(LocalVoxelLocation);
		}
	}
	else
	{
		FVoxelDataTableRow* DataTableRow = GetWorldInstance()->GetVoxelDataRow(VoxelMaterial);
		if (DataTableRow)
		{
			if (DataTableRow->bIsMesh)
			{
				FVector PlantGlobalLocation = ChunkUtil::ConvertVoxelOffsetToGlobalVoxelLocation(LocalVoxelLocation, Location, VoxelSize);
				PlantGlobalLocation.X += 50;
				PlantGlobalLocation.Y += 50;

				if (!PlantActors.Contains(VoxelMaterial))
				{
					UStaticMesh* PlantMesh = DataTableRow->Mesh;
					UMaterialInstance* PlantMaterial = DataTableRow->MaterialInstance;

					if (!PlantMesh || !PlantMaterial) return;

					APlantBase* Plant = GetWorld()->SpawnActor<APlantBase>(PlantGlobalLocation, FRotator::ZeroRotator);
					Plant->SetMesh(PlantMesh, PlantMaterial);

					PlantActors.Add(VoxelMaterial, Plant);
				}

				PlantActors[VoxelMaterial]->AddMesh(FTransform(PlantGlobalLocation), LocalVoxelLocation);
			}
		}
	}
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
	UCubicWorldInstance* WorldInstance = Cast<UCubicWorldInstance>(GetGameInstance());
	if (!WorldInstance)
	{
		return;
	}

	for (auto & Pair : MeshData)
	{
		int32 SectionIndex = (int32)Pair.Key - 1;
		FChunkMesh* ChunkMesh = Pair.Value;
		if (!ChunkMesh) continue;

		FVoxelDataTableRow* VoxelDataTableRow = WorldInstance->GetVoxelDataRow(Pair.Key);
		if (!VoxelDataTableRow) continue;

		UMaterialInstance* VoxelMaterial = VoxelDataTableRow->MaterialInstance;
		if (!VoxelMaterial) continue;

		ProceduralMeshComponent->CreateMeshSection(SectionIndex, ChunkMesh->Vertices, ChunkMesh->Triangles, ChunkMesh->Normals, ChunkMesh->UVs, ChunkMesh->VertexColors, ChunkMesh->Tangents, true);
		ProceduralMeshComponent->SetMaterial(SectionIndex, VoxelMaterial);

	}
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Chunk Mesh Generated: %s"), *GetFName().ToString()));
}

void AChunk::SetVoxelData(const TArray<FVoxel>& VoxelData)
{
	this->VoxelData = VoxelData;
}

void AChunk::SetPlantData(const TMap<EVoxelMaterial, TArray<FIntVector>>& PlantData)
{
	for (auto & Pair : PlantData)
	{
		FVoxelDataTableRow* VoxelDataTableRow = GetWorldInstance()->GetVoxelDataRow(Pair.Key);
		if (!VoxelDataTableRow) continue;
		if (!VoxelDataTableRow->bIsMesh) continue;

		UStaticMesh* PlantMesh = VoxelDataTableRow->Mesh;
		UMaterialInstance* PlantMaterial = VoxelDataTableRow->MaterialInstance;

		if (!PlantMesh || !PlantMaterial) continue;

		for (auto & Coord : Pair.Value)
		{
			FVector GlobalLocation = ChunkUtil::ConvertVoxelOffsetToGlobalVoxelLocation(Coord, Location, VoxelSize);
			GlobalLocation.X += 50;
			GlobalLocation.Y += 50;

			if (!PlantActors.Contains(Pair.Key))
			{
				APlantBase* Plant = GetWorld()->SpawnActor<APlantBase>(GlobalLocation, FRotator::ZeroRotator);
				Plant->SetMesh(PlantMesh, PlantMaterial);

				PlantActors.Add(Pair.Key, Plant);
			}
			PlantActors[Pair.Key]->AddMesh(FTransform(GlobalLocation), Coord);
		}
	}
}
