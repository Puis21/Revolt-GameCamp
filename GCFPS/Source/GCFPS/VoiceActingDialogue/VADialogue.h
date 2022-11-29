// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Sound/SoundWave.h"
#include "VADialogue.generated.h"

USTRUCT( BlueprintType )
struct FVADialogue : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	// Voice Acting sound
	UPROPERTY( EditAnywhere )
	USoundWave* SFX;

	// Voice Acting subtitles - use this to shoe subtitles in the HUD
	UPROPERTY( EditAnywhere )
	TArray<FString> Subtitles;

	// Length the subtitle should appear for - Use lenght of the SFX
	UPROPERTY( EditAnywhere )
	float AssociatedTime;
};
