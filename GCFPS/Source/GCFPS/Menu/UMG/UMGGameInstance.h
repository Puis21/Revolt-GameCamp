#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "GCFPS/Music/EMusic.h"

#include "GCFPS/VoiceActingDialogue/VADialogue.h"

#include "UMGGameInstance.generated.h"

// Forward Declarations
class UTextBlock;

UCLASS()
class GCFPS_API UUMGGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Constructor to initialize our Widget Objects //
	UUMGGameInstance(const FObjectInitializer& ObjectInitializer);

	// Init Function for Debug purposes //
	virtual void Init();

	virtual void OnWorldChanged( UWorld* OldWorld,UWorld* NewWorld ) override;

	// Individual Functions to Show Widgets //
	void ShowMenu();
	void ShowPause();
	void ShowGameOver();
	void ShowLevelComplete();
	void ShowOptions();

	void ShowStarting2DCutscene();

	// Individual Functions to Remove Widgets //
	void RemoveMenu();
	void RemovePause();
	void RemoveGameOver();
	void RemoveLevelComplete();
	void RemoveOptions();

	void RemoveStarting2DCutscene();

	// Function to quit the game //
	void QuitGame();

	UFUNCTION(BlueprintCallable)
	void SetMusicToPlay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAudioComponent* BGMusicAmbience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAudioComponent* BGMusicCombat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAudioComponent* BGMusicChase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MouseSensitivity;

	UAudioComponent* m_pCurrentAudio;

	UPROPERTY(EditAnywhere, Category = FadeDuration)
	float FadeDuration;

	void PlayNewMusic( EMusic eNewMusic, float VolumeToFade );
	void FadeToSilence();

	void QuietenMusic();
	void LoudenMusic();

	// Options Menu Functions
	void SetMusicVolume(float NewVolume);
	void SetSFXVolume(float NewVolume);
	void SetDialogueVolume(float NewVolume);

	void SetFieldOfView(float SliderValue);

private:
	// Forward Declared Variables //
	float SFXVolumeToSet;

	float MusicValueToSet;
	float SFXValueToSet;
	float DialogueValueToSet;
	float MouseSensitivityValue;

	float NewFOV;

	bool SubtitleCheck;
	bool FullScreenCheck;

	TSubclassOf<class UUserWidget> MainMenuWidgetClass;
	TSubclassOf<class UUserWidget> PauseMenuWidgetClass;
	TSubclassOf<class UUserWidget> GameOverWidgetClass;
	TSubclassOf<class UUserWidget> LevelCompleteWidgetClass;
	TSubclassOf<class UUserWidget> OptionsWidgetClass;
	TSubclassOf<class UUserWidget> Starting2DCutsceneWidgetClass;

	class UUserWidget* MainMenu;
	class UUserWidget* PauseMenu;
	class UUserWidget* GameOverMenu;
	class UUserWidget* LevelComplete;
	class UUserWidget* OptionsMenu;
	class UUserWidget* Starting2DCutscene;

	UPROPERTY(EditAnywhere, Category = MusicToSet)
	class USoundWave* AmbienceMusic;

	UPROPERTY(EditAnywhere, Category = MusicToSet)
	class USoundWave* CombatMusic;

	UPROPERTY(EditAnywhere, Category = MusicToSet)
	class USoundWave* ChaseMusic;

	// Booleans to check for conditions //
	bool bIsGameOverActive;
	bool bIsLevelCompleteActive;

public:
	UUserWidget* GetStarting2DCutscene() { return Starting2DCutscene; }

// ----------------------------------------------------------------
	// Voice Acting Dialogue

private:
	UPROPERTY( EditAnywhere, Category = "Dialogue System" )
	class UDataTable* m_pVADialogueScript;

	UAudioComponent* m_pCurrentVA;

	UAudioComponent* m_pCurrentCutsceneVA;

public:

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UAudioComponent* VACutscene1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UAudioComponent* VAStartLevel;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UAudioComponent* VAEndWave1;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UAudioComponent* VAEndWave2;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UAudioComponent* VAEndWave3;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UAudioComponent* VAEndWave4;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UAudioComponent* VAEndWave5;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
		class UAudioComponent* VAEndWave6;

private:

	FTimerHandle VASubTimerHandle;

	float VATimeToNextSub;

	UFUNCTION()
		void SetNewVASubtitle();

	int m_iVADialogueScriptCount;

	int iSubArrayLength;

	UAudioComponent* m_pVACutsceneDialogueAudio;

	int m_iVACutsceneDialogueCount;

	FTimerHandle CutsceneSubTimerHandle;

	float CutsceneTimeToNextSub;

	UFUNCTION()
		void SetNewCutsceneSubtitle();

	FTimerHandle ClearSubTimerHandle;

	float m_fTimeToClearSub;

	UFUNCTION()
		void ClearSubtitles();

	bool m_bCanClearSubs;

	int m_iVASubtitlesCount;

	FString m_subtitlesText;

	UFUNCTION()
		void AudioFinished();

	bool m_bCanSkipVA;

	bool m_bCanSkipCutsceneVA;

	bool m_bIsInCutscene;

public:
	UFUNCTION( BlueprintCallable )
	void SetVAToPlay();

	USoundWave* GetVASFX(int iVAToPlay);

	// Get subtitles in the DT - at index 0
	FString GetVASubtitles( int iVASubtitles );

	void SetSubtitles( int iVASubtitlesIndex );

	void PlayCutsceneVA();

	void PlayNextVA();

	void StopVA();

	void StopCutsceneVADialogue();

	FTimerHandle GetCutsceneTimerHandle() { return CutsceneSubTimerHandle; }

	FVADialogue* GetDialogue( UDataTable* VADialogueTable, FName RowName );

	UDataTable* GetVADialogueScript() { return m_pVADialogueScript; }

	int GetVADialogueScriptCount() { return m_iVADialogueScriptCount; }
	void SetDialogueScriptCount(int newVAEncounterCount) { m_iVADialogueScriptCount = newVAEncounterCount; }

	bool GetIsInCutscene() { return m_bIsInCutscene; }
	void SetIsInCutscene( bool bIsInCutscene ) { m_bIsInCutscene = bIsInCutscene; }


	void SetSubtitlesText( UTextBlock* SubText );
	FString GetSubtitlesText() { return m_subtitlesText; }

	void GameOverResetVA();
	void MainMenuRestartVA();

	void ClearCutsceneTimer();

	void ResetSubtitlesCount();

	void ClearSubtitlesText();

	// Logan - Options Getters and Setters
	void SetMusicVolumeSliderValue(float SliderValue) { MusicValueToSet = SliderValue; }
	float GetMusicVolumeSliderValue() { return MusicValueToSet; }

	void SetSFXVolumeSliderValue(float SliderValue) { SFXValueToSet = SliderValue; }
	float GetSFXVolumeSliderValue() { return SFXValueToSet; }

	void SetDialogueVolumeSliderValue(float SliderValue) { DialogueValueToSet = SliderValue; }
	float GetDialogueVolumeSliderValue() { return DialogueValueToSet; }
	
	void SetSubtitlesBool(bool SubtitlesOn) { SubtitleCheck = SubtitlesOn; }
	bool GetSubtitlesBool() { return SubtitleCheck; }

	void SetFullscreenBool(bool isFullScreen) { FullScreenCheck = isFullScreen; }
	bool GetFullscreenBool() { return FullScreenCheck; }

	void SetMouseSensitivity(float NewSensitivity) { MouseSensitivity = NewSensitivity; }
	void SetMouseSensitivityValue(float NewValue) { MouseSensitivityValue = NewValue; }
	float GetMouseSensitivityValue() { return MouseSensitivityValue; }

	float GetFOVValue() { return NewFOV; }
	float GetSFXVolumeValue() { return SFXVolumeToSet; }
};