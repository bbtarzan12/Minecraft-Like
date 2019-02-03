// Fill out your copyright notice in the Description page of Project Settings.

#include "ChunkMeshTask.h"
#include "MineCraft.h"
#include "Chunk.h"
#include "ProceduralMeshComponent.h"
#include "SimplexNoiseBPLibrary.h"
#include "Public/ChunkUtil.h"
#include "Async.h"

const int32 bBackTriangles[] = { 2,3,1, 1,0,2 };
const int32 bFrontTriangles[] = { 2,0,1, 1,3,2 };
const FVector bNormals[][4] =
{
	{ FVector(1, 0, 0),  FVector(1, 0, 0),  FVector(1, 0, 0),  FVector(1, 0, 0)  },
	{ FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) },
	{ FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) },
	{ FVector(0, -1, 0),  FVector(0, -1, 0),  FVector(0, -1, 0),  FVector(0, -1, 0)  },
	{ FVector(0, 0, 1),  FVector(0, 0, 1),  FVector(0, 0, 1),  FVector(0, 0, 1)  },
	{ FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) }
};
const FProcMeshTangent bTangents[][4] =
{
	{FProcMeshTangent(0, 1, 0),  FProcMeshTangent(0, 1, 0),  FProcMeshTangent(0, 1, 0),  FProcMeshTangent(0, 1, 0)  },
	{FProcMeshTangent(0, -1, 0), FProcMeshTangent(0, -1, 0), FProcMeshTangent(0, -1, 0), FProcMeshTangent(0, -1, 0) },
	{FProcMeshTangent(-1, 0, 0), FProcMeshTangent(-1, 0, 0), FProcMeshTangent(-1, 0, 0), FProcMeshTangent(-1, 0, 0) },
	{FProcMeshTangent(1, 0, 0),  FProcMeshTangent(1, 0, 0),  FProcMeshTangent(1, 0, 0),  FProcMeshTangent(1, 0, 0)  },
	{FProcMeshTangent(0, 1, 0),  FProcMeshTangent(0, 1, 0),  FProcMeshTangent(0, 1, 0),  FProcMeshTangent(0, 1, 0)  },
	{FProcMeshTangent(0, -1, 0), FProcMeshTangent(0, -1, 0), FProcMeshTangent(0, -1, 0), FProcMeshTangent(0, -1, 0) }
};


ChunkMeshTask::ChunkMeshTask(AChunk* Owner, const FIntVector& ChunkOffset, const FIntVector& ChunkSize, const int32& VoxelSize, const float& NoiseWeight, const float& NoiseScale, const int32& RandomSeed)
{

	this->NoiseWeight = NoiseWeight;
	this->NoiseScale = NoiseScale;
	this->RandomSeed = RandomSeed;
	this->ChunkOffset = ChunkOffset;
	this->ChunkSize = ChunkSize;
	this->VoxelSize = VoxelSize;
	this->Owner = Owner;
	bIsFirstTime = true;
}

ChunkMeshTask::ChunkMeshTask(AChunk* Owner, const FIntVector& ChunkSize, const int32& VoxelSize, const TArray<FVoxelFace>& VoxelData, const int32& Index, const EVoxelType& VoxelType)
	: Owner(Owner), ChunkSize(ChunkSize), VoxelSize(VoxelSize), VoxelData(VoxelData)
{
	VoxelEditIndex = Index;
	VoxelEditType = VoxelType;
	bIsFirstTime = false;
}

ChunkMeshTask::~ChunkMeshTask()
{
}

FORCEINLINE TStatId ChunkMeshTask::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ChunkMeshTask, STATGROUP_ThreadPoolAsyncTasks);
}

void ChunkMeshTask::DoWork()
{
	if (bIsFirstTime)
	{
		GenerateChunk();
	}
	else
	{
		VoxelData[VoxelEditIndex].Type = VoxelEditType;
		VoxelData[VoxelEditIndex].IsValid = true;
	}
	UpdateMesh();

	AsyncTask(ENamedThreads::GameThread, [&]()
	{
		if (Owner != nullptr)
		{
			Owner->SetVoxelData(VoxelData);
			Owner->GenerateMesh(MeshData);
		}
	});
}

FVoxelFace ChunkMeshTask::GetVoxelFace(int32 X, int32 Y, int32 Z, EFaceDirection Side)
{
	int32 Index = ChunkUtil::Convert3Dto1DIndex(X, Y, Z, ChunkSize);
	VoxelData[Index].Side = Side;
	VoxelData[Index].Transparent = false;
	FIntVector OppositeCoord = ChunkUtil::GetNeighborIndex(X, Y, Z, (int32)Side);
	int32 NeighborIndex = ChunkUtil::Convert3Dto1DIndex(OppositeCoord, ChunkSize);
	if (ChunkUtil::BoundaryCheck3D(OppositeCoord, ChunkSize))
	{
		if (VoxelData[NeighborIndex].Type != EVoxelType::NONE && VoxelData[NeighborIndex].Type != EVoxelType::LEAVES)
		{
			VoxelData[Index].Transparent = true;
		}
	}

	return VoxelData[Index];
}

void ChunkMeshTask::GenerateChunk()
{
	FIntVector ChunkLocation;
	ChunkLocation.X = ChunkOffset.X * ChunkSize.X;
	ChunkLocation.Y = ChunkOffset.Y * ChunkSize.Y;
	ChunkLocation.Z = ChunkOffset.Z * ChunkSize.Z;

	VoxelData.SetNumUninitialized(ChunkSize.X * ChunkSize.Y * ChunkSize.Z);
	USimplexNoiseBPLibrary::setNoiseSeed(RandomSeed);
	FRandomStream RandomStream = FRandomStream(RandomSeed + ChunkLocation.X + ChunkLocation.Y + ChunkLocation.Z);

	TArray<FIntVector> TreeRoots;

	for (int32 X = 0; X < ChunkSize.X; X++)
	{
		for (int32 Y = 0; Y < ChunkSize.Y; Y++)
		{
			for (int32 Z = 0; Z < ChunkSize.Z; Z++)
			{
				int32 Index = ChunkUtil::Convert3Dto1DIndex(X, Y, Z, ChunkSize);

				FIntVector CurrentChunkLocation;
				CurrentChunkLocation.X = ChunkLocation.X + X;
				CurrentChunkLocation.Y = ChunkLocation.Y + Y;
				CurrentChunkLocation.Z = ChunkLocation.Z + Z;

				float GroundValue = USimplexNoiseBPLibrary::SimplexNoiseScaledFractal2D(CurrentChunkLocation.X, CurrentChunkLocation.Y, 0.01f, 3, 4.0f, 32.0f) * 3.0f;
				float MountainValue = USimplexNoiseBPLibrary::SimplexNoiseScaledFractal2D(CurrentChunkLocation.X, CurrentChunkLocation.Y, 0.01f, 5, 0.2f, 256.0f, 1.0f, 1.0f) * 64.0f;
				float Mountain3DMask = USimplexNoiseBPLibrary::SimplexNoiseScaledFractal3D(CurrentChunkLocation.X, CurrentChunkLocation.Y, CurrentChunkLocation.Z, 0.05f);
				if (MountainValue < 0)
					MountainValue = 1;

				float NoiseValue = GroundValue + MountainValue;

				FVoxelFace VoxelFace = FVoxelFace();

				if (20 < CurrentChunkLocation.Z && Mountain3DMask > 0.4f)
				{
					VoxelFace.Type = EVoxelType::NONE;
					VoxelFace.IsValid = false;
				}
				else if (NoiseValue + 8 > CurrentChunkLocation.Z)
				{
					VoxelFace.Type = EVoxelType::COBBLESTONE;
					VoxelFace.IsValid = true;
				}
				else if (NoiseValue + 9 > CurrentChunkLocation.Z)
				{
					VoxelFace.Type = EVoxelType::DIRT;
					VoxelFace.IsValid = true;
				}
				else if (NoiseValue + 10 > CurrentChunkLocation.Z)
				{
					VoxelFace.Type = EVoxelType::GRASS;
					VoxelFace.IsValid = true;
				}
				else if (NoiseValue + 11 > CurrentChunkLocation.Z && RandomStream.FRand() < 0.1f)
				{
					VoxelData[Index].Type = EVoxelType::LOG;
					VoxelData[Index].IsValid = true;
					TreeRoots.Add(FIntVector(X, Y, Z));
				}
				else
				{
					VoxelFace.Type = EVoxelType::NONE;
					VoxelFace.IsValid = false;
				}
				VoxelData[Index] = VoxelFace;
			}
		}
	}

	for (const auto & Root : TreeRoots)
	{
		FIntVector MinBound = FIntVector(Root.X - 2, Root.Y - 2, Root.Z);
		FIntVector MaxBound = FIntVector(Root.X + 2, Root.Y + 2, Root.Z + 6);

		bool IsTreeValid = true;
		for (int32 X = MinBound.X; X <= MaxBound.X && IsTreeValid; X++)
		{
			for (int32 Y = MinBound.Y; Y <= MaxBound.Y && IsTreeValid; Y++)
			{
				for (int32 Z = MinBound.Z; Z <= MaxBound.Z && IsTreeValid; Z++)
				{
					if (ChunkUtil::BoundaryCheck3D(X, Y, Z, ChunkSize))
					{
						if ((Z == Root.Z || Z == Root.Z + 1) && !(X == Root.X && Y == Root.Z))
						{
							continue;
						}

						int32 Index = ChunkUtil::Convert3Dto1DIndex(X, Y, Z, ChunkSize);
						if (VoxelData[Index].Type == EVoxelType::NONE)
						{
							continue;
						}
					}

					IsTreeValid = false;
				}
			}
		}

		if (IsTreeValid)
		{
			for (int32 Z = 0; Z < 5; Z++)
			{
				int32 Index = ChunkUtil::Convert3Dto1DIndex(Root.X, Root.Y, Root.Z + Z, ChunkSize);
				VoxelData[Index].Type = EVoxelType::LOG;
				VoxelData[Index].IsValid = true;
			}

			for (int32 X = -1; X <= 1; X++)
			{
				for (int32 Y = -1; Y <= 1; Y++)
				{
					for (int32 Z = 2; Z < 6; Z++)
					{
						int32 Index = ChunkUtil::Convert3Dto1DIndex(Root.X + X, Root.Y + Y, Root.Z + Z, ChunkSize);

						if ((X != 0 || Y != 0) || (Z == 5 && X == 0 && Y == 0))
						{
							VoxelData[Index].Type = EVoxelType::LEAVES;
							VoxelData[Index].IsValid = true;
						}
					}
				}
			}


		}
		else
		{
			int32 Index = ChunkUtil::Convert3Dto1DIndex(Root.X, Root.Y, Root.Z, ChunkSize);
			VoxelData[Index].Type = EVoxelType::NONE;
			VoxelData[Index].IsValid = false;
		}
	}
}

void ChunkMeshTask::UpdateMesh()
{
	int32 i, j, k, l, w, h, u, v, n;
	EFaceDirection Side = EFaceDirection::LEFT;

	TArray<FVoxelFace> Mask;
	Mask.SetNumUninitialized(ChunkSize.X * ChunkSize.Y);

	FVoxelFace voxelFace;
	FVoxelFace voxelFace1;

	// lesser-spotted boolean for-loop
	for (bool BackFace = true, B = false; B != BackFace; BackFace = BackFace && B, B = !B)
	{
		/*
		 * We sweep over the 3 dimensions - most of what follows is well described by Mikola Lysenko
		 * in his post - and is ported from his Javascript implementation.  Where this implementation
		 * diverges, I've added commentary.
		 */
		for (int32 Dimension = 0; Dimension < 3; Dimension++)
		{

			u = (Dimension + 1) % 3;
			v = (Dimension + 2) % 3;

			TArray<int32> X = { 0,0,0 };
			TArray<int32> Q = { 0,0,0 };

			Q[Dimension] = 1;

			/*
			 * Here we're keeping track of the side that we're meshing.
			 */
			if (Dimension == 0) { Side = BackFace ? EFaceDirection::BACK : EFaceDirection::FRONT; }
			else if (Dimension == 1) { Side = BackFace ? EFaceDirection::LEFT : EFaceDirection::RIGHT; }
			else if (Dimension == 2) { Side = BackFace ? EFaceDirection::BOTTOM : EFaceDirection::TOP; }

			/*
			 * We move through the dimension from front to back
			 */
			for (X[Dimension] = -1; X[Dimension] < ChunkSize.X;)
			{

				/*
				 * -------------------------------------------------------------------
				 *   We compute the mask
				 * -------------------------------------------------------------------
				 */
				n = 0;

				for (X[v] = 0; X[v] < ChunkSize.Y; X[v]++)
				{

					for (X[u] = 0; X[u] < ChunkSize.X; X[u]++)
					{

						/*
						 * Here we retrieve two voxel faces for comparison.
						 */
						voxelFace = (X[Dimension] >= 0) ? GetVoxelFace(X[0], X[1], X[2], Side) : FVoxelFace();
						voxelFace1 = (X[Dimension] < ChunkSize.X - 1) ? GetVoxelFace(X[0] + Q[0], X[1] + Q[1], X[2] + Q[2], Side) : FVoxelFace();

						/*
						 * Note that we're using the equals function in the voxel face class here, which lets the faces
						 * be compared based on any number of attributes.
						 *
						 * Also, we choose the face to add to the mask depending on whether we're moving through on a backface or not.
						 */
						Mask[n++] = ((voxelFace.IsValid && voxelFace1.IsValid && voxelFace == voxelFace1)) ? FVoxelFace() : BackFace ? voxelFace1 : voxelFace;
					}
				}

				X[Dimension]++;

				/*
				 * Now we generate the mesh for the mask
				 */
				n = 0;

				for (j = 0; j < ChunkSize.Y; j++)
				{

					for (i = 0; i < ChunkSize.X;)
					{

						if (Mask[n].IsValid && Mask[n].Type != EVoxelType::NONE)
						{

							/*
							 * We compute the width
							 */
							for (w = 1; i + w < ChunkSize.X && Mask[n + w].IsValid && Mask[n + w] == Mask[n]; w++) {}

							/*
							 * Then we compute height
							 */
							bool done = false;

							for (h = 1; j + h < ChunkSize.Y; h++)
							{

								for (k = 0; k < w; k++)
								{

									if (!Mask[n + k + h * ChunkSize.X].IsValid || !(Mask[n + k + h * ChunkSize.X] == Mask[n])) { done = true; break; }
								}

								if (done) { break; }
							}

							/*
							 * Here we check the "transparent" attribute in the VoxelFace class to ensure that we don't mesh
							 * any culled faces.
							 */
							if (!Mask[n].Transparent)
							{
								/*
								 * Add quad
								 */
								X[u] = i;
								X[v] = j;

								TArray<int32> DU = { 0,0,0 };
								TArray<int32> DV = { 0,0,0 };

								DU[u] = w;
								DV[v] = h;

								/*
								 * And here we call the quad function in order to render a merged quad in the scene.
								 *
								 * We pass mask[n] to the function, which is an instance of the VoxelFace class containing
								 * all the attributes of the face - which allows for variables to be passed to shaders - for
								 * example lighting values used to create ambient occlusion.
								 */
								UpdateQuad
								(
									FVector(X[0], X[1], X[2]),
									FVector(X[0] + DU[0], X[1] + DU[1], X[2] + DU[2]),
									FVector(X[0] + DU[0] + DV[0], X[1] + DU[1] + DV[1], X[2] + DU[2] + DV[2]),
									FVector(X[0] + DV[0], X[1] + DV[1], X[2] + DV[2]),
									w,
									h,
									Mask[n],
									BackFace
								);
							}

							/*
							 * We zero out the mask
							 */
							for (l = 0; l < h; ++l)
							{

								for (k = 0; k < w; ++k) { Mask[n + k + l * ChunkSize.X] = FVoxelFace(); }
							}

							/*
							 * And then finally increment the counters and continue
							 */
							i += w;
							n += w;

						}
						else
						{

							i++;
							n++;
						}
					}
				}
			}
		}
	}
}

void ChunkMeshTask::UpdateQuad(FVector BottomLeft, FVector TopLeft, FVector TopRight, FVector BottomRight, int32 Width, int32 Height, FVoxelFace VoxelFace, bool BackFace)
{
	FChunkMesh* ChunkMesh;
	if (MeshData.Contains(VoxelFace.Type))
	{
		ChunkMesh = MeshData[VoxelFace.Type];
	}
	else
	{
		ChunkMesh = new FChunkMesh();
		MeshData.Add(VoxelFace.Type, ChunkMesh);
	}


	ChunkMesh->Vertices.Add(BottomLeft * VoxelSize);
	ChunkMesh->Vertices.Add(BottomRight * VoxelSize);
	ChunkMesh->Vertices.Add(TopLeft * VoxelSize);
	ChunkMesh->Vertices.Add(TopRight * VoxelSize);

	ChunkMesh->Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[0] + ChunkMesh->NumTriangle);
	ChunkMesh->Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[1] + ChunkMesh->NumTriangle);
	ChunkMesh->Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[2] + ChunkMesh->NumTriangle);
	ChunkMesh->Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[3] + ChunkMesh->NumTriangle);
	ChunkMesh->Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[4] + ChunkMesh->NumTriangle);
	ChunkMesh->Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[5] + ChunkMesh->NumTriangle);
	ChunkMesh->NumTriangle += 4;

	FColor Color = FColor::White;
	ChunkMesh->VertexColors.Add(Color);
	ChunkMesh->VertexColors.Add(Color);
	ChunkMesh->VertexColors.Add(Color);
	ChunkMesh->VertexColors.Add(Color);

	switch (VoxelFace.Side)
	{
		case EFaceDirection::FRONT:
		{
			ChunkMesh->UVs.Add(FVector2D(Width, Height));
			ChunkMesh->UVs.Add(FVector2D(Width, 0));
			ChunkMesh->UVs.Add(FVector2D(0, Height));
			ChunkMesh->UVs.Add(FVector2D(0, 0));
			break;
		}
		case EFaceDirection::BACK:
		{
			ChunkMesh->UVs.Add(FVector2D(0, Height));
			ChunkMesh->UVs.Add(FVector2D(0, 0));
			ChunkMesh->UVs.Add(FVector2D(Width, Height));
			ChunkMesh->UVs.Add(FVector2D(Width, 0));
			break;
		}
		case EFaceDirection::RIGHT:
		{
			ChunkMesh->UVs.Add(FVector2D(0, Width));
			ChunkMesh->UVs.Add(FVector2D(Height, Width));
			ChunkMesh->UVs.Add(FVector2D(0, 0));
			ChunkMesh->UVs.Add(FVector2D(Height, 0));
			break;
		}
		case EFaceDirection::LEFT:
		{
			ChunkMesh->UVs.Add(FVector2D(Height, Width));
			ChunkMesh->UVs.Add(FVector2D(0, Width));
			ChunkMesh->UVs.Add(FVector2D(Height, 0));
			ChunkMesh->UVs.Add(FVector2D(0, 0));
			break;
		}
		case EFaceDirection::TOP:
		{
			ChunkMesh->UVs.Add(FVector2D(0, 0));
			ChunkMesh->UVs.Add(FVector2D(0, Height));
			ChunkMesh->UVs.Add(FVector2D(Width, 0));
			ChunkMesh->UVs.Add(FVector2D(Width, Height));
			break;
		}
		case EFaceDirection::BOTTOM:
		{
			ChunkMesh->UVs.Add(FVector2D(Width, 0));
			ChunkMesh->UVs.Add(FVector2D(Width, Height));
			ChunkMesh->UVs.Add(FVector2D(0, 0));
			ChunkMesh->UVs.Add(FVector2D(0, Height));
			break;
		}
		default:
			break;
	}

	ChunkMesh->Normals.Append(bNormals[(int32)VoxelFace.Side], ARRAY_COUNT(bNormals[(int32)VoxelFace.Side]));
	ChunkMesh->Tangents.Append(bTangents[(int32)VoxelFace.Side], ARRAY_COUNT(bTangents[(int32)VoxelFace.Side]));
}
