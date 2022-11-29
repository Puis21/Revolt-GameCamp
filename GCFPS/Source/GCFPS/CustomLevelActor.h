// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "CustomLevelActor.generated.h"

/**
 * 
 */
UCLASS()
class GCFPS_API ACustomLevelActor : public ALevelScriptActor
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Fade", meta = (AllowPrivateAccess = "true"))
	float FadeVolume = 1.0f;
};
