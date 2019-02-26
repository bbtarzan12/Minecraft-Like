// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Chunk.h"
#include "CubicWorldInstance.generated.h"

struct FVoxelDataTableRow;
class UDataTable;

UCLASS()
class CUBICWORLD_API UCubicWorldInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UCubicWorldInstance(const FObjectInitializer& ObjectInitializer);
	
	FVoxelDataTableRow* GetVoxelDataRow(const EVoxelMaterial& VoxelMaterial);
	UDataTable* GetRawVoxelDataTable();

private:
	static UDataTable* VoxelDataTable;
	static TMap<EVoxelMaterial, FVoxelDataTableRow*> DataTableCache;
};
