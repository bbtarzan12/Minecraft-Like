// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

static class MINECRAFT_API VoxelUtil
{
public:
	static int32 Convert3Dto1DIndex(FIntVector Coord, FIntVector Size);
	static int32 Convert3Dto1DIndex(int32 X, int32 Y, int32 Z, FIntVector Size);

	static FIntVector Convert1Dto3DIndex(int32 Index, FIntVector Size);

	static bool BoundaryCheck3D(FIntVector Coord, FIntVector Size);
	static bool BoundaryCheck3D(int32 X, int32 Y, int32 Z, FIntVector Size);

	static FIntVector GetNeighborIndex(int32 Index, int32 Direction, FIntVector Size);
	static FIntVector GetNeighborIndex(FIntVector Coord, int32 Direction);
	static FIntVector GetNeighborIndex(int32 X, int32 Y, int32 Z, int32 Direction);

	static FIntVector ConvertGlobalToLocal(FVector GlobalLocation, FVector ChunkLocation, int32 Size);
};