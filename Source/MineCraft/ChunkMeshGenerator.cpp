#include "ChunkMeshGenerator.h"
#include "Chunk.h"
#include "MineCraft.h"
#include "SimplexNoiseBPLibrary.h"
#include "ProceduralMeshComponent.h"

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


bool FChunkMeshGenerator::IsFinished() const
{
	return bIsFinished;
}

FChunkMeshGenerator::FChunkMeshGenerator(const FIntVector& ChunkLocation, const FIntVector& ChunkSize, const int32& VoxelSize, const float& NoiseWeight, const float& NoiseScale, const int32& RandomSeed)
{
	this->NoiseWeight = NoiseWeight;
	this->NoiseScale = NoiseScale;
	this->RandomSeed = RandomSeed;
	this->ChunkLocation = ChunkLocation;
	this->ChunkSize = ChunkSize;
	this->VoxelSize = VoxelSize;
	Thread = FRunnableThread::Create(this, TEXT("FChunkMeshGenerator"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FChunkMeshGenerator::~FChunkMeshGenerator()
{
	delete Thread;
	Thread = nullptr;
}

//Init
bool FChunkMeshGenerator::Init()
{
	bIsFinished = false;
	return true;
}

//Run
uint32 FChunkMeshGenerator::Run()
{
	GenerateChunk();
	UpdateMesh();
	bIsFinished = true;
	return 0;
}

//stop
void FChunkMeshGenerator::Stop()
{
	StopTaskCounter.Increment();
}

FVoxelFace FChunkMeshGenerator::GetVoxelFace(int32 X, int32 Y, int32 Z, EFaceDirection Side)
{
	int32 Index = X + (Y * ChunkSize.X) + (Z * ChunkSize.X * ChunkSize.Y);
	VoxelData[Index].Side = Side;
	return VoxelData[Index];
}

void FChunkMeshGenerator::GenerateChunk()
{
	VoxelData.SetNumUninitialized(ChunkSize.X * ChunkSize.Y * ChunkSize.Z);
	USimplexNoiseBPLibrary::setNoiseSeed(RandomSeed);

	for (int32 X = 0; X < ChunkSize.X; X++)
	{
		for (int32 Y = 0; Y < ChunkSize.Y; Y++)
		{
			for (int32 Z = 0; Z < ChunkSize.Z; Z++)
			{
				int32 Index = X + (Y * ChunkSize.X) + (Z * ChunkSize.X * ChunkSize.Y);

				FIntVector CurrentChunkLocation;
				CurrentChunkLocation.X = ChunkLocation.X * ChunkSize.X + X;
				CurrentChunkLocation.Y = ChunkLocation.Y * ChunkSize.Y + Y;
				CurrentChunkLocation.Z = ChunkLocation.Z * ChunkSize.Z + Z;

				FVector ScaledNoiseOffset;
				ScaledNoiseOffset.X = CurrentChunkLocation.X * NoiseScale;
				ScaledNoiseOffset.Y = CurrentChunkLocation.Y * NoiseScale;

				float NoiseValue = USimplexNoiseBPLibrary::SimplexNoiseScaled2D(ScaledNoiseOffset.X, ScaledNoiseOffset.Y, NoiseWeight);
				FVoxelFace VoxelFace = FVoxelFace();
				VoxelFace.IsValid = NoiseValue > CurrentChunkLocation.Z;
				VoxelData[Index] = VoxelFace;
			}
		}
		FPlatformProcess::Sleep(0.01);
	}
}

void FChunkMeshGenerator::UpdateMesh()
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

						if (Mask[n].IsValid)
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

								DU[0] = 0;
								DU[1] = 0;
								DU[2] = 0;
								DU[u] = w;

								DV[0] = 0;
								DV[1] = 0;
								DV[2] = 0;
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
			FPlatformProcess::Sleep(0.01);
		}
	}
}

void FChunkMeshGenerator::UpdateQuad(FVector BottomLeft, FVector TopLeft, FVector TopRight, FVector BottomRight, int32 Width, int32 Height, FVoxelFace VoxelFace, bool BackFace)
{
	Vertices.Add(BottomLeft * VoxelSize);
	Vertices.Add(BottomRight * VoxelSize);
	Vertices.Add(TopLeft * VoxelSize);
	Vertices.Add(TopRight * VoxelSize);

	Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[0] + NumTriangle);
	Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[1] + NumTriangle);
	Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[2] + NumTriangle);
	Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[3] + NumTriangle);
	Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[4] + NumTriangle);
	Triangles.Add((BackFace ? bBackTriangles : bFrontTriangles)[5] + NumTriangle);
	NumTriangle += 4;

	FColor Color = FColor(255, 255, 255, 255);
	VertexColors.Add(Color);
	VertexColors.Add(Color);
	VertexColors.Add(Color);
	VertexColors.Add(Color);

	switch (VoxelFace.Side)
	{
		case EFaceDirection::FRONT:
		{
			UVs.Add(FVector2D(Width, Height));
			UVs.Add(FVector2D(Width, 0));
			UVs.Add(FVector2D(0, Height));
			UVs.Add(FVector2D(0, 0));
			break;
		}
		case EFaceDirection::BACK:
		{
			UVs.Add(FVector2D(0, Height));
			UVs.Add(FVector2D(0, 0));
			UVs.Add(FVector2D(Width, Height));
			UVs.Add(FVector2D(Width, 0));
			break;
		}
		case EFaceDirection::RIGHT:
		{
			UVs.Add(FVector2D(0, Width));
			UVs.Add(FVector2D(Height, Width));
			UVs.Add(FVector2D(0, 0));
			UVs.Add(FVector2D(Height, 0));
			break;
		}
		case EFaceDirection::LEFT:
		{
			UVs.Add(FVector2D(Height, Width));
			UVs.Add(FVector2D(0, Width));
			UVs.Add(FVector2D(Height, 0));
			UVs.Add(FVector2D(0, 0));
			break;
		}
		case EFaceDirection::TOP:
		{
			UVs.Add(FVector2D(0, 0));
			UVs.Add(FVector2D(0, Height));
			UVs.Add(FVector2D(Width, 0));
			UVs.Add(FVector2D(Width, Height));
			break;
		}
		case EFaceDirection::BOTTOM:
		{
			UVs.Add(FVector2D(Width, 0));
			UVs.Add(FVector2D(Width, Height));
			UVs.Add(FVector2D(0, 0));
			UVs.Add(FVector2D(0, Height));
			break;
		}
		default:
			break;
	}

	Normals.Append(bNormals[(int32)VoxelFace.Side], ARRAY_COUNT(bNormals[(int32)VoxelFace.Side]));
	Tangents.Append(bTangents[(int32)VoxelFace.Side], ARRAY_COUNT(bTangents[(int32)VoxelFace.Side]));
}