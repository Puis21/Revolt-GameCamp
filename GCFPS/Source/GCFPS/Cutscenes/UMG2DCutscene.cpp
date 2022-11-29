// Fill out your copyright notice in the Description page of Project Settings.

#include "UMG2DCutscene.h"

#include "Animation/WidgetAnimation.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

#include "Blueprint/UserWidget.h"


void UUMG2DCutscene::NativeConstruct()
{
	Super::NativeConstruct();

	ContinueButton->OnClicked.AddUniqueDynamic( this, &UUMG2DCutscene::OnContinueButton );

	EndDelegate.BindDynamic( this, &UUMG2DCutscene::AnimationFinishedCanvasFadeOut );
	BindToAnimationFinished( AnimCanvasFadeOut, EndDelegate );
			
	CurrentImage->SetBrushFromTexture( CutsceneImage );
}

void UUMG2DCutscene::OnContinueButton()
{
	PlayAnimation( AnimCanvasFadeOut );
	UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>( UGameplayStatics::GetGameInstance( GetWorld() ) );
	if (pGameInstance)
	{
		pGameInstance->StopCutsceneVADialogue();
	}
}

void UUMG2DCutscene::AnimationFinishedCanvasFadeOut()
{
	UUMGGameInstance* pGameInstance = Cast<UUMGGameInstance>( UGameplayStatics::GetGameInstance( GetWorld() ) );
	if (pGameInstance)
	{
		pGameInstance->GameOverResetVA();
		pGameInstance->RemoveStarting2DCutscene();
		UGameplayStatics::OpenLevel( this, MapName );
	}
}

void UUMG2DCutscene::UpdateSubtitlesText( FString SubText )
{
	SubtitlesText->SetText( FText::FromString( SubText ) );
}

void UUMG2DCutscene::ClearSubtitlesText()
{
	SubtitlesText->SetText( FText::FromString( " " ) );
}
