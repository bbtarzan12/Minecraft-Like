#pragma once

#include "ProceduralMeshComponent.h"
#include "VoxelCommon.generated.h"

UENUM(BlueprintType)
enum class EVoxelMaterial : uint8
{
	NONE, GRASS, DIRT, COBBLESTONE, LOG, LEAVES, TALLGRASS
};

USTRUCT(BlueprintType)
struct FVoxel
{
	GENERATED_USTRUCT_BODY()
	
	FIntVector Coord;
	EVoxelMaterial Material = EVoxelMaterial::NONE;
	bool bIsMesh = false;
};

USTRUCT()
struct FChunkMesh
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FVector> Vertices;
	UPROPERTY()
	TArray<int32> Triangles;
	UPROPERTY()
	TArray<FVector> Normals;
	UPROPERTY()
	TArray<FVector2D> UVs;
	UPROPERTY()
	TArray<FProcMeshTangent> Tangents;
	UPROPERTY()
	TArray<FColor> VertexColors;
	UPROPERTY()
	int32 NumTriangle;
};