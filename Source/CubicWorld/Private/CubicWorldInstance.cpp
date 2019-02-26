// Fill out your copyright notice in the Description page of Project Settings.

#include "CubicWorldInstance.h"
#include <ConstructorHelpers.h>
#include <Engine/DataTable.h>
#include "VoxelData.h"
#include "CubicWorld.h"

UDataTable* UCubicWorldInstance::VoxelDataTable = nullptr;

TMap<EVoxelMaterial, FVoxelDataTableRow*> UCubicWorldInstance::DataTableCache;

UCubicWorldInstance::UCubicWorldInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UDataTable> VoxelDataTableObj(TEXT("DataTable'/Game/Data/VoxelDataTable.VoxelDataTable'"));
	if (VoxelDataTableObj.Succeeded() && VoxelDataTable == nullptr)
	{
		VoxelDataTable = VoxelDataTableObj.Object;
	}
}

FVoxelDataTableRow * UCubicWorldInstance::GetVoxelDataRow(const EVoxelMaterial& VoxelMaterial)
{
	if (!DataTableCache.Contains(VoxelMaterial))
	{
		check(VoxelDataTable);
		FString EnumName = GETENUMSTRING("EVoxelMaterial", VoxelMaterial);
		FName EnumFName = FName(*EnumName);
		FVoxelDataTableRow* DataTableRow = VoxelDataTable->FindRow<FVoxelDataTableRow>(EnumFName, EnumName);
		check(DataTableRow);
		DataTableCache.Add(VoxelMaterial, DataTableRow);
	}

	return DataTableCache[VoxelMaterial];
}

UDataTable* UCubicWorldInstance::GetRawVoxelDataTable()
{
	return VoxelDataTable;
}
