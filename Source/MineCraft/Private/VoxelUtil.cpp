// Fill out your copyright notice in the Description page of Project Settings.

#include "Public/VoxelUtil.h"

int32 VoxelUtil::Convert3Dto1DIndex(FIntVector Coord, FIntVector Size)
{
	return Convert3Dto1DIndex(Coord.X, Coord.Y, Coord.Z, Size);
}

int32 VoxelUtil::Convert3Dto1DIndex(int32 X, int32 Y, int32 Z, FIntVector Size)
{
	return  X + (Y * Size.X) + (Z * Size.X * Size.Y);
}

FIntVector VoxelUtil::Convert1Dto3DIndex(int32 Index, FIntVector Size)
{
	return FIntVector(Index % Size.X, (Index / Size.X) % Size.Y, ((Index / Size.X) / Size.Y) % Size.Z);
}

bool VoxelUtil::BoundaryCheck3D(int32 X, int32 Y, int32 Z, FIntVector Size)
{
	return X >= 0 && X < Size.X && Y >= 0 && Y < Size.Y && Z >= 0 && Z < Size.Z;
}

bool VoxelUtil::BoundaryCheck3D(FIntVector Coord, FIntVector Size)
{
	return BoundaryCheck3D(Coord.X, Coord.Y, Coord.Z, Size);
}

FIntVector VoxelUtil::GetNeighborIndex(int32 X, int32 Y, int32 Z, int32 Direction)
{
	switch (Direction)
	{
		case 0: return FIntVector(X + 1, Y, Z);
		case 1: return FIntVector(X - 1, Y, Z);
		case 2: return FIntVector(X, Y + 1, Z);
		case 3: return FIntVector(X, Y - 1, Z);
		case 4: return FIntVector(X, Y, Z + 1);
		case 5: return FIntVector(X, Y, Z - 1);
		default: return FIntVector(0, 0, 0);
	}
}

FIntVector VoxelUtil::GetNeighborIndex(FIntVector Coord, int32 Direction)
{
	return GetNeighborIndex(Coord.X, Coord.Y, Coord.Z, Direction);
}

FIntVector VoxelUtil::GetNeighborIndex(int32 Index, int32 Direction, FIntVector Size)
{
	FIntVector Coord = Convert1Dto3DIndex(Index, Size);
	return GetNeighborIndex(Coord, Direction);
}

FIntVector VoxelUtil::ConvertGlobalToLocal(FVector GlobalLocation, FVector ChunkLocation, int32 Size)
{
	FIntVector LocalChunkLocation = FIntVector(GlobalLocation - ChunkLocation);
	LocalChunkLocation.X /= Size;
	LocalChunkLocation.Y /= Size;
	LocalChunkLocation.Z /= Size;

	return LocalChunkLocation;
}
