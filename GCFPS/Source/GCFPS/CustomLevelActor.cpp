// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomLevelActor.h"
#include "Menu/UMG/UMGGameInstance.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void ACustomLevelActor::BeginPlay()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(GetGameInstance());
	GameInstance->SetMusicToPlay();
	GameInstance->SetVAToPlay();
	GameInstance->PlayNewMusic( EMusic::Background, FadeVolume );
}
