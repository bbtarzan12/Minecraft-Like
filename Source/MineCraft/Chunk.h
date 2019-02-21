// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Chunk.generated.h"

class ChunkMeshTask;
class APlant;
struct FChunkMesh;

UENUM()
enum class EFaceDirection : uint8
{
	FRONT, BACK, RIGHT, LEFT, TOP, BOTTOM,
};

UENUM()
enum class EVoxelMaterial : uint8
{
	NONE, GRASS, DIRT, COBBLESTONE, LOG, LEAVES, TALLGRASS
};

UENUM()
enum class EVoxelType : uint8
{
	NONE, Voxel, PlantMesh
};


USTRUCT()
struct FVoxelFace
{
	GENERATED_USTRUCT_BODY()
	
	FIntVector Coord;
	bool Transparent = false;
	EVoxelMaterial Material = EVoxelMaterial::NONE;
	EVoxelType Type;
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

UCLASS()
class MINECRAFT_API AChunk : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	int32 RandomSeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FIntVector ChunkSize = FIntVector(32, 32, 32);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	float NoiseScale = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	float NoiseWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	int32 VoxelSize = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FIntVector ChunkOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = Chunk)
	FVector Location;

	UPROPERTY()
	int32 VoxelSizeHalf;

	UPROPERTY()
	TArray<FVoxelFace> VoxelData;

	UPROPERTY()
	TArray<APlant*> PlantData;

private:
	static TMap<EVoxelMaterial, UMaterialInstanceDynamic*> VoxelMaterials;
	void StartTask();
	void StartTask(int32 Index, EVoxelMaterial VoxelMaterial, EVoxelType VoxelType);
	void GeneratePlantMesh(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial, const EVoxelType& VoxelType);

public:
	// Sets default values for this actor's properties
	AChunk();

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	void SetVoxel(const FVector& GlobalLocation, const EVoxelMaterial& VoxelMaterial, const EVoxelType& VoxelType);
	void Init(int32 RandomSeed, FIntVector ChunkSize, float NoiseScale, float NoiseWeight, int32 VoxelSize);
	void GenerateMesh(const TMap<EVoxelMaterial, FChunkMesh*>& MeshData);
	void SetVoxelData(const TArray<FVoxelFace>& VoxelData);
};
