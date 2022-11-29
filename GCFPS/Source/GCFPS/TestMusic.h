// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "TestMusic.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GCFPS_API UTestMusic : public UAudioComponent
{
	GENERATED_BODY()

	UTestMusic(const FObjectInitializer& ObjectInitializer);

public:
	UFUNCTION(BlueprintCallable)
	void PlayMusic();

	UFUNCTION(BlueprintCallable)
	void SwitchMusic();

private:
	class USoundWave* ambienceSound;
	class USoundWave* chaseSound;
	
};
