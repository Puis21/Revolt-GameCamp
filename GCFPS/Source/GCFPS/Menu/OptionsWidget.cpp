#include "OptionsWidget.h"
#include "GCFPS/Menu/UMG/UMGGameInstance.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetStringLibrary.h"
#include "GameFramework/GameUserSettings.h"

void UOptionsWidget::NativeConstruct()
{
	ControlsButton->OnClicked.AddUniqueDynamic(this, &UOptionsWidget::OnControlsButtonClicked);
	VideoButton->OnClicked.AddUniqueDynamic(this, &UOptionsWidget::OnVideoButtonClicked);
	AudioButton->OnClicked.AddUniqueDynamic(this, &UOptionsWidget::OnAudioButtonClicked);
	RestoreButton->OnClicked.AddUniqueDynamic(this, &UOptionsWidget::OnRestoreButtonClicked);
	BackButton->OnClicked.AddUniqueDynamic(this, &UOptionsWidget::OnBackButtonClicked);
	FullScreenCheck->OnCheckStateChanged.AddUniqueDynamic(this, &UOptionsWidget::OnFullScreenToggle);
	WindowedCheck->OnCheckStateChanged.AddUniqueDynamic(this, &UOptionsWidget::OnWindowedToggle);
	SubtitlesCheck->OnCheckStateChanged.AddUniqueDynamic(this, &UOptionsWidget::OnSubtitleToggle);
	ResolutionCombo->OnSelectionChanged.AddUniqueDynamic(this, &UOptionsWidget::OnSelectionBoxChange);
	MusicVolumeSlider->OnValueChanged.AddUniqueDynamic(this, &UOptionsWidget::OnMusicSliderChange);
	SFXVolumeSlider->OnValueChanged.AddUniqueDynamic(this, &UOptionsWidget::OnSFXSliderChange);
	DialogueVolumeSlider->OnValueChanged.AddUniqueDynamic(this, &UOptionsWidget::OnDialogueSliderChange);
	FOVSlider->OnValueChanged.AddUniqueDynamic(this, &UOptionsWidget::OnFOVSliderChange);
	MouseSensitivitySlider->OnValueChanged.AddUniqueDynamic(this, &UOptionsWidget::OnMouseSliderChange);
	
	GameInstance = Cast<UUMGGameInstance>(GetGameInstance());

	myGameSettings = UGameUserSettings::GetGameUserSettings();

	MusicVolumeSlider->SetValue(GameInstance->GetMusicVolumeSliderValue());
	SFXVolumeSlider->SetValue(GameInstance->GetSFXVolumeSliderValue());
	DialogueVolumeSlider->SetValue(GameInstance->GetDialogueVolumeSliderValue());
	FOVSlider->SetValue(GameInstance->GetFOVValue());
	MouseSensitivitySlider->SetValue(GameInstance->GetMouseSensitivityValue());

	if (GameInstance->GetSubtitlesBool())
	{
		SubtitlesCheck->SetCheckedState(ECheckBoxState::Checked);
	}
	else 
	{
		SubtitlesCheck->SetCheckedState(ECheckBoxState::Unchecked);
	}
	
	if (GameInstance->GetFullscreenBool())
	{
		FullScreenCheck->SetCheckedState(ECheckBoxState::Checked);
		WindowedCheck->SetCheckedState(ECheckBoxState::Unchecked);
	}
	else
	{
		FullScreenCheck->SetCheckedState(ECheckBoxState::Unchecked);
		WindowedCheck->SetCheckedState(ECheckBoxState::Checked);
	}
}

void UOptionsWidget::Construct()
{
}

void UOptionsWidget::OnControlsButtonClicked()
{
	CloseAllPanels();
	ControlsPanel->SetVisibility(ESlateVisibility::Visible);
}

void UOptionsWidget::OnVideoButtonClicked()
{
	CloseAllPanels();
	VideoPanel->SetVisibility(ESlateVisibility::Visible);
}

void UOptionsWidget::OnAudioButtonClicked()
{
	CloseAllPanels();
	AudioPanel->SetVisibility(ESlateVisibility::Visible);
}

void UOptionsWidget::OnRestoreButtonClicked()
{
	FullScreenCheck->SetCheckedState(ECheckBoxState::Checked);
	WindowedCheck->SetCheckedState(ECheckBoxState::Unchecked);
	SubtitlesCheck->SetCheckedState(ECheckBoxState::Checked);
	GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	GameInstance->SetFullscreenBool(true);
	GameInstance->SetSubtitlesBool(true);
	myGameSettings->SetScreenResolution(FIntPoint(1920, 1080));
	GameInstance->SetMusicVolume(1.0f);
	GameInstance->SetMusicVolumeSliderValue(100.0f);
	GameInstance->SetSFXVolume(1.0f);
	GameInstance->SetSFXVolumeSliderValue(100.0f);
	GameInstance->SetDialogueVolume(1.0f);
	GameInstance->SetDialogueVolumeSliderValue(100.0f);
	GameInstance->SetFieldOfView(90.0f);
	GameInstance->SetMouseSensitivity(1.0f);
	GameInstance->SetMouseSensitivityValue(10.0f);

	MusicVolumeSlider->SetValue(GameInstance->GetMusicVolumeSliderValue());
	SFXVolumeSlider->SetValue(GameInstance->GetSFXVolumeSliderValue());
	DialogueVolumeSlider->SetValue(GameInstance->GetDialogueVolumeSliderValue());
	FOVSlider->SetValue(GameInstance->GetFOVValue());
	MouseSensitivitySlider->SetValue(GameInstance->GetMouseSensitivityValue());

	myGameSettings->ApplySettings(true);
}

void UOptionsWidget::OnBackButtonClicked()
{
	GameInstance->RemoveOptions();
	GameInstance->ShowMenu();
}

void UOptionsWidget::OnFullScreenToggle(bool bIsChecked)
{
	if (bIsChecked)
	{
		WindowedCheck->SetCheckedState(ECheckBoxState::Unchecked);
		GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		GameInstance->SetFullscreenBool(true);
	}
	else
	{
		WindowedCheck->SetCheckedState(ECheckBoxState::Checked);
		GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
		GameInstance->SetFullscreenBool(false);
	}

	myGameSettings->ApplySettings(true);
}

void UOptionsWidget::OnWindowedToggle(bool bIsChecked)
{
	if (bIsChecked)
	{
		FullScreenCheck->SetCheckedState(ECheckBoxState::Unchecked);
		GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::Windowed);
		GameInstance->SetFullscreenBool(false);
	}
	else
	{
		FullScreenCheck->SetCheckedState(ECheckBoxState::Checked);
		GEngine->GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		GameInstance->SetFullscreenBool(true);
	}

	myGameSettings->ApplySettings(true);
}

void UOptionsWidget::OnSubtitleToggle(bool bIsChecked)
{
		GameInstance->SetSubtitlesBool(bIsChecked);
}

void UOptionsWidget::OnSelectionBoxChange(FString selectedResolution, ESelectInfo::Type selectType)
{
	int32 leftString;
	if (selectedResolution.Len() == 9)
	{
		leftString = UKismetStringLibrary::Conv_StringToInt(selectedResolution.LeftChop(5));
	}
	else
	{
		leftString = UKismetStringLibrary::Conv_StringToInt(selectedResolution.LeftChop(4));
	}
	
	int32 rightString = UKismetStringLibrary::Conv_StringToInt(selectedResolution.RightChop(5));


	NewResolution = FIntPoint(leftString, rightString);
	myGameSettings->SetScreenResolution(NewResolution);
	myGameSettings->ApplySettings(true);
}

void UOptionsWidget::OnMusicSliderChange(float NewVolume)
{
	float volumePercentage = NewVolume / 100.0f;
	GameInstance->SetMusicVolume(volumePercentage);
	GameInstance->SetMusicVolumeSliderValue(NewVolume);
}

void UOptionsWidget::OnSFXSliderChange(float NewVolume)
{
	float volumePercentage = NewVolume / 100.0f;
	GameInstance->SetSFXVolume(volumePercentage);
	GameInstance->SetSFXVolumeSliderValue(NewVolume);
}

void UOptionsWidget::OnDialogueSliderChange(float NewVolume)
{
	float volumePercentage = NewVolume / 100.0f;
	GameInstance->SetDialogueVolume(volumePercentage);
	GameInstance->SetDialogueVolumeSliderValue(NewVolume);
}

void UOptionsWidget::OnFOVSliderChange(float NewValue)
{
	GameInstance->SetFieldOfView(NewValue);
}

void UOptionsWidget::OnMouseSliderChange(float NewValue)
{
	float valuePercentage = NewValue / 10.0f;
	GameInstance->SetMouseSensitivity(valuePercentage);
	GameInstance->SetMouseSensitivityValue(NewValue);
}

void UOptionsWidget::CloseAllPanels()
{
	ControlsPanel->SetVisibility(ESlateVisibility::Hidden);
	VideoPanel->SetVisibility(ESlateVisibility::Hidden);
	AudioPanel->SetVisibility(ESlateVisibility::Hidden);
}
