// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VoxelCommon.h"
#include "VoxelData.h"
#include "ChunkMeshTask.generated.h"

class AChunk;
struct FChunkMesh;
class UDataTable;
enum class EVoxelMaterial : uint8;

UENUM()
enum class EFaceDirection : uint8
{
	FRONT, BACK, RIGHT, LEFT, TOP, BOTTOM,
};

USTRUCT()
struct FVoxelFace
{
	GENERATED_USTRUCT_BODY()

	FIntVector Coord;
	bool Transparent = false;
	EVoxelMaterial Material;
	EFaceDirection Side;
	bool IsOpacity = false;
	bool HasMesh = false;

	FORCEINLINE bool operator==(const FVoxelFace & Other) const
	{
		if (HasMesh || Other.HasMesh)
			return false;
		return Transparent == Other.Transparent && Material == Other.Material && IsOpacity == Other.IsOpacity;
	}
};

class CUBICWORLD_API ChunkMeshTask : public FNonAbandonableTask
{
	friend class FAsyncTask<ChunkMeshTask>;

public:
	ChunkMeshTask(AChunk* Owner, const FIntVector& ChunkOffset, const FIntVector& ChunkSize, const int32& VoxelSize, const float& NoiseWeight, const float& NoiseScale, const int32& RandomSeed, UDataTable* VoxelDataTable);
	ChunkMeshTask(AChunk* Owner, const FIntVector& ChunkSize, const int32& VoxelSize, const TArray<FVoxel>& VoxelData, const int32& Index, const EVoxelMaterial& VoxelMaterial, UDataTable* VoxelDataTable);
	~ChunkMeshTask();

	bool IsTaskFinished();

	UPROPERTY()
	TMap<EVoxelMaterial, TArray<FIntVector>> PlantData; // 처음 청크를 만들때만 사용
	UPROPERTY()
	TMap<EVoxelMaterial, FChunkMesh*> MeshData;
	UPROPERTY()
	TArray<FVoxel> ResultVoxelData;

	int32 VoxelEditIndex = -1;
	EVoxelMaterial VoxelEditMaterial;

private:
	UPROPERTY()
	TArray<FVoxelFace> VoxelData;
	UPROPERTY()
	static TMap<EVoxelMaterial, FVoxelDataTableRow*> DataTableCache;

	AChunk* Owner;

	bool bIsTaskFinished;
	bool bIsFirstTime;

	FIntVector ChunkOffset;
	FIntVector ChunkSize;
	int32 VoxelSize = 0;
	float NoiseWeight = 0;
	float NoiseScale = 0;
	int32 RandomSeed = 0;

	UDataTable* VoxelDataTable;

protected:
	FORCEINLINE TStatId GetStatId() const;
	void DoWork();

private:
	FVoxelDataTableRow* GetVoxelDataRow(const EVoxelMaterial& VoxelMaterial);
	void SetVoxelFace(FVoxelFace& Face, const EVoxelMaterial& Material);
	FVoxelFace GetVoxelFace(int32 X, int32 Y, int32 Z, EFaceDirection Side);
	void GenerateChunk();
	void UpdateMesh();
	void UpdateQuad(FVector BottomLeft, FVector TopLeft, FVector TopRight, FVector BottomRight, int32 Width, int32 Height, FVoxelFace VoxelFace, bool BackFace);
};