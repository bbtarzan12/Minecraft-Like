// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Chunk.h"
#include "ChunkMeshTask.generated.h"


struct FProcMeshTangent;

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

class MINECRAFT_API ChunkMeshTask : public FNonAbandonableTask
{

public:
	ChunkMeshTask(AChunk* Owner, const FIntVector& ChunkOffset, const FIntVector& ChunkSize, const int32& VoxelSize, const float& NoiseWeight, const float& NoiseScale, const int32& RandomSeed);
	ChunkMeshTask(AChunk* Owner, const FIntVector& ChunkSize, const int32& VoxelSize, const TArray<FVoxelFace>& VoxelData, const int32& Index, const EVoxelType& VoxelType);
	~ChunkMeshTask();

	TArray<FVoxelFace> VoxelData;

	TMap<EVoxelType, FChunkMesh*> MeshData;
	AChunk* Owner;

	FIntVector ChunkOffset;
	FIntVector ChunkSize;
	int32 VoxelSize = 0;
	float NoiseWeight = 0;
	float NoiseScale = 0;
	int32 RandomSeed = 0;

	int32 VoxelEditIndex = -1;
	EVoxelType VoxelEditType;

private:
	bool bIsFirstTime;

public:
	FORCEINLINE TStatId GetStatId() const;
	void DoWork();

	FVoxelFace GetVoxelFace(int32 X, int32 Y, int32 Z, EFaceDirection Side);
	void GenerateChunk();
	void UpdateMesh();
	void UpdateQuad(FVector BottomLeft, FVector TopLeft, FVector TopRight, FVector BottomRight, int32 Width, int32 Height, FVoxelFace VoxelFace, bool BackFace);

};
