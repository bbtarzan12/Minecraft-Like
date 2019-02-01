// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Chunk.generated.h"

class FChunkMeshGenerator;

UENUM()
enum class EFaceDirection : uint8
{
	FRONT, BACK, RIGHT, LEFT, TOP, BOTTOM,
};

UENUM()
enum class EVoxelType : uint8
{
	GRASS, DIRT, COBBLESTONE
};

USTRUCT()
struct FVoxelFace
{
	GENERATED_USTRUCT_BODY()
	
	bool Transparent;
	EVoxelType Type;
	EFaceDirection Side;
	bool IsValid;

	FORCEINLINE bool operator==(const FVoxelFace & Other) const
	{
		return IsValid == true && Other.IsValid == true && Transparent == Other.Transparent && Type == Other.Type;
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
	FIntVector ChunkLocation;

	UPROPERTY()
	int32 VoxelSizeHalf;

	UPROPERTY()
	TArray <FVoxelFace> ChunkField;

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;

private:
	FChunkMeshGenerator* Worker;

	static TArray<UMaterialInstanceDynamic*> VoxelMaterials;

public:
	// Sets default values for this actor's properties
	AChunk();

protected:
	// Called when the game starts or when spawned
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;

public:

	virtual void Init(int32 RandomSeed, FIntVector ChunkSize, float NoiseScale, float NoiseWeight, int32 VoxelSize);
};
