// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsWidget.generated.h"

/**
 * 
 */

class UUMGGameInstance;

UCLASS()
class GCFPS_API UOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeConstruct() override;

	void Construct();

	UFUNCTION()
	void OnControlsButtonClicked();
	UFUNCTION()
	void OnVideoButtonClicked();
	UFUNCTION()
	void OnAudioButtonClicked();
	UFUNCTION()
	void OnRestoreButtonClicked();
	UFUNCTION()
	void OnBackButtonClicked();
	UFUNCTION()
	void OnFullScreenToggle(bool bIsChecked);
	UFUNCTION()
	void OnWindowedToggle(bool bIsChecked);
	UFUNCTION()
	void OnSubtitleToggle(bool bIsChecked);
	UFUNCTION()
	void OnSelectionBoxChange(FString selectedResolution, ESelectInfo::Type selectType);
	UFUNCTION()
	void OnMusicSliderChange(float NewVolume);
	UFUNCTION()
	void OnSFXSliderChange(float NewVolume);
	UFUNCTION()
	void OnDialogueSliderChange(float NewVolume);
	UFUNCTION()
	void OnFOVSliderChange(float NewValue);
	UFUNCTION()
	void OnMouseSliderChange(float NewValue);

	void CloseAllPanels();

	UPROPERTY(meta = (BindWidget))
	class UButton* ControlsButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* VideoButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* AudioButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* RestoreButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* ControlsPanel;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* VideoPanel;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* AudioPanel;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UComboBoxString* ResolutionCombo;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UCheckBox* FullScreenCheck;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UCheckBox* WindowedCheck;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class UCheckBox* SubtitlesCheck;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USlider* MusicVolumeSlider;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USlider* SFXVolumeSlider;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USlider* DialogueVolumeSlider;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USlider* FOVSlider;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	class USlider* MouseSensitivitySlider;

	// Widget Animations
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ControlFadeIn;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ControlFadeOut;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* VideoFadeIn;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* VideoFadeOut;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* AudioFadeIn;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* AudioFadeOut;

private:
	UUMGGameInstance* GameInstance;
	UGameUserSettings* myGameSettings;
	FIntPoint NewResolution;

};
