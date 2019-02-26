// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <Engine/DataTable.h>
#include <Materials/MaterialInstance.h>
#include "VoxelCommon.h"
#include "VoxelData.generated.h"

UCLASS()
class CUBICWORLD_API UVoxelData : public UObject
{
	GENERATED_BODY()
	
};


USTRUCT(BlueprintType)
struct FVoxelDataTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	EVoxelMaterial Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	bool bIsOpacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	bool bIsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel", meta = (EditCondition = "bIsMesh"))
	UStaticMesh* Mesh;
};