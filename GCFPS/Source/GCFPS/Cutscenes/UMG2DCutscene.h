// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UMG2DCutscene.generated.h"

// Forward Declarations
class UButton;
class UImage;
class UTextBlock;
class UTexture2D;
class UWidgetAnimation;

UCLASS()
class GCFPS_API UUMG2DCutscene : public UUserWidget
{
	GENERATED_BODY()

private:
	FWidgetAnimationDynamicEvent EndDelegate;

	UFUNCTION()
		void AnimationFinishedCanvasFadeOut();

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void OnContinueButton();

	UPROPERTY( meta = (BindWidget) )
	UButton* ContinueButton;

	// First map/level to go to 
	UPROPERTY( EditAnywhere )
	FName MapName;

	UPROPERTY( EditAnywhere, Category = "Cutscene Image" );
	UTexture2D* CutsceneImage;

	UPROPERTY( BlueprintReadOnly, meta = (BindWidget) )
	UTextBlock* SubtitlesText;

	UPROPERTY( BlueprintReadOnly, meta = (BindWidget) )
	UImage* CurrentImage;

	UPROPERTY( meta = (BindWidgetAnim), Transient )
	UWidgetAnimation* AnimCanvasFadeOut;

	void UpdateSubtitlesText( FString SubText );

	void ClearSubtitlesText();
		
};
