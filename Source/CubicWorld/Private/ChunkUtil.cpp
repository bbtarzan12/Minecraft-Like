// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkUtil.h"

int32 ChunkUtil::Convert3Dto1DIndex(FIntVector Coord, FIntVector Size)
{
	return Convert3Dto1DIndex(Coord.X, Coord.Y, Coord.Z, Size);
}

int32 ChunkUtil::Convert3Dto1DIndex(int32 X, int32 Y, int32 Z, FIntVector Size)
{
	return  X + (Y * Size.X) + (Z * Size.X * Size.Y);
}

FIntVector ChunkUtil::Convert1Dto3DIndex(int32 Index, FIntVector Size)
{
	return FIntVector(Index % Size.X, (Index / Size.X) % Size.Y, ((Index / Size.X) / Size.Y) % Size.Z);
}

bool ChunkUtil::BoundaryCheck3D(int32 X, int32 Y, int32 Z, FIntVector Size)
{
	return X >= 0 && X < Size.X && Y >= 0 && Y < Size.Y && Z >= 0 && Z < Size.Z;
}

bool ChunkUtil::BoundaryCheck3D(FIntVector Coord, FIntVector Size)
{
	return BoundaryCheck3D(Coord.X, Coord.Y, Coord.Z, Size);
}

bool ChunkUtil::BoundaryCheckBox(FIntVector Coord, FIntVector Min, FIntVector Max)
{
	return BoundaryCheckBox(Coord.X, Coord.Y, Coord.Z, Min, Max);
}

bool ChunkUtil::BoundaryCheckBox(int32 X, int32 Y, int32 Z, FIntVector Min, FIntVector Max)
{
	return
		X >= Min.X &&
		X <= Max.X &&
		Y >= Min.Y &&
		Y <= Max.Y &&
		Z >= Min.Z &&
		Z <= Max.Z;
}

FIntVector ChunkUtil::GetNeighborIndex(int32 X, int32 Y, int32 Z, int32 Direction)
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

FIntVector ChunkUtil::GetNeighborIndex(FIntVector Coord, int32 Direction)
{
	return GetNeighborIndex(Coord.X, Coord.Y, Coord.Z, Direction);
}

FIntVector ChunkUtil::GetNeighborIndex(int32 Index, int32 Direction, FIntVector Size)
{
	FIntVector Coord = Convert1Dto3DIndex(Index, Size);
	return GetNeighborIndex(Coord, Direction);
}

FIntVector ChunkUtil::ConvertGlobalToLocal(FVector GlobalLocation, FVector ChunkLocation, int32 Size)
{
	FVector LocalChunkLocation = GlobalLocation - ChunkLocation;
	LocalChunkLocation.X = FMath::FloorToInt(LocalChunkLocation.X / Size);
	LocalChunkLocation.Y = FMath::FloorToInt(LocalChunkLocation.Y / Size);
	LocalChunkLocation.Z = FMath::FloorToInt(LocalChunkLocation.Z / Size);

	return FIntVector(LocalChunkLocation);
}

FVector ChunkUtil::ConvertOffsetToChunkLocation(FIntVector Offset, FIntVector ChunkSize, int32 VoxelSize)
{
	FVector ChunkLocation;
	ChunkLocation.X = Offset.X * ChunkSize.X * VoxelSize;
	ChunkLocation.Y = Offset.Y * ChunkSize.Y * VoxelSize;
	ChunkLocation.Z = Offset.Z * ChunkSize.Z * VoxelSize;
	return ChunkLocation;
}

FVector ChunkUtil::ConvertVoxelOffsetToGlobalVoxelLocation(FIntVector VoxelOffset, FVector ChunkLocation, int32 VoxelSize)
{
	FVector VoxelLocation;
	VoxelLocation.X = VoxelOffset.X * VoxelSize + ChunkLocation.X;
	VoxelLocation.Y = VoxelOffset.Y * VoxelSize + ChunkLocation.Y;
	VoxelLocation.Z = VoxelOffset.Z * VoxelSize + ChunkLocation.Z;
	return VoxelLocation;
}

FIntVector ChunkUtil::ConvertLocationToOffset(FVector Location, FIntVector ChunkSize, int32 VoxelSize)
{
	FIntVector Offset;
	Offset.X = FMath::FloorToInt(Location.X / VoxelSize / ChunkSize.X);
	Offset.Y = FMath::FloorToInt(Location.Y / VoxelSize / ChunkSize.Y);
	Offset.Z = FMath::FloorToInt(Location.Z / VoxelSize / ChunkSize.Z);

	return Offset;
}
