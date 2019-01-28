// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Chunk.h"
#include "ProceduralMeshComponent.h"

class FChunkMeshGenerator : public FRunnable
{

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

	bool bIsFinished;

public:
	TArray<FVoxelFace> VoxelData;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;

	FIntVector ChunkLocation;
	FIntVector ChunkSize;
	int32 NumTriangle = 0;
	int32 VoxelSize = 0;
	float NoiseWeight = 0;
	float NoiseScale = 0;
	int32 RandomSeed = 0;

	//Done?
	bool IsFinished() const;

	//~~~ Thread Core Functions ~~~

	//Constructor / Destructor
	FChunkMeshGenerator(const FIntVector& ChunkLocation, const FIntVector& ChunkSize, const int32& VoxelSize, const float& NoiseWeight, const float& NoiseScale, const int32& RandomSeed);
	virtual ~FChunkMeshGenerator();

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	// End FRunnable interface

	FVoxelFace GetVoxelFace(int32 X, int32 Y, int32 Z, EFaceDirection Side);
	void GenerateChunk();
	void UpdateMesh();
	void UpdateQuad(FVector BottomLeft, FVector TopLeft, FVector TopRight, FVector BottomRight, int32 Width, int32 Height, FVoxelFace VoxelFace, bool BackFace);
};