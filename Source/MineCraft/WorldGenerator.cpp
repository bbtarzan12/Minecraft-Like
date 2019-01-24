// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldGenerator.h"
#include "SimplexNoiseBPLibrary.h"

const int32 bTriangles[] =	{ 2, 1, 0, 0, 3, 2 };
const FVector2D bUVs[] =	{ FVector2D(0, 0), FVector2D(0, 1), FVector2D(1, 1), FVector2D(1, 0) };
const FVector bNormals0[] = { FVector(0, 0, 1), FVector(0, 0, 1),  FVector(0, 0, 1), FVector(0, 0, 1) };
const FVector bNormals1[] = { FVector(0, 0, -1),FVector(0, 0, -1), FVector(0, 0, -1),FVector(0, 0, -1) };
const FVector bNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0),  FVector(0, 1, 0), FVector(0, 1, 0) };
const FVector bNormals3[] = { FVector(0, -1, 0),FVector(0, -1, 0), FVector(0, -1, 0),FVector(0, -1, 0) };
const FVector bNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0),  FVector(1, 0, 0), FVector(1, 0, 0) };
const FVector bNormals5[] = { FVector(-1, 0, 0),FVector(-1, 0, 0), FVector(-1, 0, 0),FVector(-1, 0, 0) };
const FVector bMask[] = { FVector(0,0,1), FVector(0,0,-1), FVector(0,1,0), FVector(0,-1,0), FVector(1,0,0), FVector(-1,0,0) };

// Sets default values
AWorldGenerator::AWorldGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AWorldGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FName ProceduralMeshComponentName = FName("ProceduralMeshComponent");
	ProceduralMeshComponent = NewObject<UProceduralMeshComponent>(this, ProceduralMeshComponentName);
	ProceduralMeshComponent->RegisterComponent();

	FVector Location = Transform.GetLocation();

	RootComponent = ProceduralMeshComponent;
	RootComponent->SetWorldLocation(Location);
	VoxelSizeHalf = VoxelSize / 2;

	ChunkLocation = FIntVector(Location);
	ChunkLocation.X /= (ChunkSize.X * VoxelSize);
	ChunkLocation.Y /= (ChunkSize.Y * VoxelSize);
	ChunkLocation.Z /= (ChunkSize.Z * VoxelSize);

	GenerateChunk();
	UpdateMesh();
}

// Called when the game starts or when spawned
void AWorldGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void AWorldGenerator::GenerateChunk()
{
	ChunkFields.SetNumUninitialized(ChunkSize.X * ChunkSize.Y * ChunkSize.Z);

	for (int32 X = 0; X < ChunkSize.X; X++)
	{
		for (int32 Y = 0; Y < ChunkSize.Y; Y++)
		{
			for (int32 Z = 0; Z < ChunkSize.Z; Z++)
			{
				int32 Index = X + (Y * ChunkSize.X) + (Z * ChunkSize.X * ChunkSize.Y);
				FIntVector Offset = ChunkLocation;
				Offset.X *= ChunkSize.X;
				Offset.Y *= ChunkSize.Y;
				Offset.Z *= ChunkSize.Z;

				float NoiseValue = USimplexNoiseBPLibrary::SimplexNoiseScaled3D(X + Offset.X, Y + Offset.Y, Z + Offset.Z, NoiseScale);
				ChunkFields[Index] = (NoiseValue > NoiseThreshold) ? 1 : 0;
				
			}
		}
	}
}

void AWorldGenerator::UpdateMesh()
{
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor> VertexColors;

	int32 ElementID = 0;

	for (int32 X = 0; X < ChunkSize.X; X++)
	{
		for (int32 Y = 0; Y < ChunkSize.Y; Y++)
		{
			for (int32 Z = 0; Z < ChunkSize.Z; Z++)
			{
				int32 Index = X + (Y * ChunkSize.X) + (Z * ChunkSize.X * ChunkSize.Y);
			 	int32 MeshIndex = ChunkFields[Index];

				if (MeshIndex > 0)
				{

					int32 NumTriangle = 0;

					for (int32 IdxMask = 0; IdxMask < 6; IdxMask++)
					{
						//int32 NewIndex = Index + bMask[IdxMask].X + (bMask[IdxMask].Y * ChunkSize.X) + (bMask[IdxMask].Z * ChunkSize.X * ChunkSize.Y);

						Triangles.Add(bTriangles[0] + NumTriangle + ElementID);
						Triangles.Add(bTriangles[1] + NumTriangle + ElementID);
						Triangles.Add(bTriangles[2] + NumTriangle + ElementID);
						Triangles.Add(bTriangles[3] + NumTriangle + ElementID);
						Triangles.Add(bTriangles[4] + NumTriangle + ElementID);
						Triangles.Add(bTriangles[5] + NumTriangle + ElementID);
						NumTriangle += 4;

						switch (IdxMask)
						{
							case 0:
							{
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));

								Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
								break;
							}
							case 1:
							{
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));

								Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
								break;
							}
							case 2:
							{
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));

								Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
								break;
							}
							case 3:
							{
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));

								Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
								break;
							}
							case 4:
							{
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));

								Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
								break;
							}
							case 5:
							{
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), -VoxelSizeHalf + (Z * VoxelSize)));
								Vertices.Add(FVector(-VoxelSizeHalf + (X * VoxelSize), -VoxelSizeHalf + (Y * VoxelSize), VoxelSizeHalf + (Z * VoxelSize)));

								Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
								break;
							}
							default:
								break;
						}

						UVs.Append(bUVs, ARRAY_COUNT(bUVs));

						FColor Color = FColor(255, 255, 255, IdxMask);
						VertexColors.Add(Color);
						VertexColors.Add(Color);
						VertexColors.Add(Color);
						VertexColors.Add(Color);

					}
					ElementID += NumTriangle;
				}
			}
		}
	}

	ProceduralMeshComponent->ClearAllMeshSections();
	ProceduralMeshComponent->CreateMeshSection(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
}

// Called every frame
void AWorldGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector playerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	//PlayerChunkPosition.X = playerLocation.X / ChunkSize.X;
	//PlayerChunkPosition.Y = playerLocation.Y / ChunkSize.Y;
	//PlayerChunkPosition.Z = playerLocation.Z / ChunkSize.Z;

	//GenerateMesh(PlayerChunkPosition);
}
