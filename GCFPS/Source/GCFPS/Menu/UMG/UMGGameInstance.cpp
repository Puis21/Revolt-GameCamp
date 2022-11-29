#include "UMGGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "UMGMenuWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/TestMusic.h"
#include "Sound/SoundWave.h"
#include "Components/TextBlock.h"
#include "GCFPS/Cutscenes/UMG2DCutscene.h"
#include "GCFPS/GCFPSHUD.h"
#include "GCFPS/Menu/HUDWidget.h"
#include "Components/AudioComponent.h"
#include "GCFPS/Player/FPCharacter.h"
#include "GCFPS/Player/Camera/FPCameraComponent.h"

// In the Constructor, we want to initialize some objects before we do anything. If these objects are not found, we prevent the game from continuing. //
// Afterwards if the program finds the objects, it will set their classes to these objects and set the booleans to "False" //
UUMGGameInstance::UUMGGameInstance(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidget(TEXT("/Game/HUD/BP_MainMenu"));
	if (!ensure(MainMenuWidget.Class != nullptr)) return;

	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuWidget(TEXT("/Game/HUD/BP_PauseMenu"));
	if (!ensure(PauseMenuWidget.Class != nullptr)) return;

	static ConstructorHelpers::FClassFinder<UUserWidget> GameOverWidget(TEXT("/Game/HUD/BP_GameOver"));
	if (!ensure(GameOverWidget.Class != nullptr)) return;

	static ConstructorHelpers::FClassFinder<UUserWidget> LevelCompleteWidget(TEXT("/Game/HUD/BP_LevelComplete"));
	if (!ensure(LevelCompleteWidget.Class != nullptr)) return;

	static ConstructorHelpers::FClassFinder<UUserWidget> OptionsWidget(TEXT("/Game/HUD/BP_Options"));
	if (!ensure(OptionsWidget.Class != nullptr)) return;

	static ConstructorHelpers::FClassFinder<UUserWidget> Starting2DCutsceneWidget( TEXT( "/Game/Cutscenes/BPW_2DCutscene" ) );
	if (!ensure( Starting2DCutsceneWidget.Class != nullptr )) return;

	static ConstructorHelpers::FObjectFinder<USoundWave> AmbienceMusicRef(TEXT("/Game/Levels/SoundAssets/BackgroundMusic"));
	if (AmbienceMusicRef.Succeeded())
	{
		AmbienceMusic = AmbienceMusicRef.Object;
		BGMusicAmbience = UGameplayStatics::CreateSound2D(this, AmbienceMusic, 1.0f, 1.0f, 0.0f, nullptr, true, false);
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> CombatMusicRef(TEXT("/Game/Levels/SoundAssets/BackgroundMusic"));
	if (CombatMusicRef.Succeeded())
	{
		CombatMusic = CombatMusicRef.Object;
		BGMusicCombat = UGameplayStatics::CreateSound2D(this, CombatMusic, 1.0f, 1.0f, 0.0f, nullptr, true, false);
	}

	static ConstructorHelpers::FObjectFinder<USoundWave> ChaseMusicRef(TEXT("/Game/Levels/SoundAssets/BackgroundMusic"));
	if (ChaseMusicRef.Succeeded())
	{
		ChaseMusic = ChaseMusicRef.Object;
		BGMusicChase = UGameplayStatics::CreateSound2D(this, ChaseMusic, 1.0f, 1.0f, 0.0f, nullptr, true, false);
	}

	MainMenuWidgetClass = MainMenuWidget.Class;
	PauseMenuWidgetClass = PauseMenuWidget.Class;
	GameOverWidgetClass = GameOverWidget.Class;
	LevelCompleteWidgetClass = LevelCompleteWidget.Class;
	OptionsWidgetClass = OptionsWidget.Class;
	Starting2DCutsceneWidgetClass = Starting2DCutsceneWidget.Class;

	FadeDuration = 3.0f;

	bIsGameOverActive = false;
	bIsLevelCompleteActive = false;

	m_iVADialogueScriptCount = 0;
	m_iVACutsceneDialogueCount = -1;
	m_iVASubtitlesCount = 0;

	m_bCanSkipVA = false;
	m_bCanSkipCutsceneVA = false;
	m_bIsInCutscene = false;
	m_bCanClearSubs = false;

	MusicValueToSet = 100.0f;
	SFXValueToSet = 100.0f;
	DialogueValueToSet = 100.0f;
	SFXVolumeToSet = 1.0f;
	NewFOV = 90.0f;
	SubtitleCheck = true;
	FullScreenCheck = true;

	MouseSensitivity = 1.0f;
	MouseSensitivityValue = 10.0f;
}

// Init function to check if the object is being found. If it has, it prints a string in the debugger //
void UUMGGameInstance::Init()
{
	UE_LOG(LogTemp, Warning, TEXT("We have found the Class %s"), *MainMenuWidgetClass->GetName());

	//if (m_pVAAudioComp)
	//{
	//	m_pVAAudioComp->OnAudioFinished.AddDynamic( this, &UUMGGameInstance::AudioFinished );
	//	//m_pVAAudioComp->OnAudioFinished.AddUObject( this, &UUMGGameInstance::AudioFinished );
	//}
	
	//m_pNewVA->OnAudioFinished.AddDynamic( this, &UUMGGameInstance::AudioFinished );

}

void UUMGGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);

	//SetVAToPlay();
	//SetMusicToPlay();
	// do shit
}

// These are the functions to show the different menus //

void UUMGGameInstance::ShowMenu()
{
	// Creating Widget and adding to viewport //
	MainMenu = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
	MainMenu->AddToViewport();

	// Get Reference to the Player Controller //
	APlayerController* PlayerController = GetFirstLocalPlayerController();

	// Set up input parameters to the SetupInputMode Function //
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(MainMenu->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	// Set Input Mode //
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UUMGGameInstance::ShowPause()
{
	// Creating Widget and adding to viewport //
	PauseMenu = CreateWidget<UUserWidget>(this, PauseMenuWidgetClass);
	PauseMenu->AddToViewport();

	// Get Reference to the Player Controller //
	APlayerController* PlayerController = GetFirstLocalPlayerController();

	// Set up input parameters to the SetupInputMode Function //
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(PauseMenu->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	// Set Input Mode and pause the Game //
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetPause(true);
}

void UUMGGameInstance::ShowGameOver()
{
	// Boolean "bIsGameOverActive" is used to check if a "GameOver" Widget is active. If it isn't, then we create a new one and add it to viewport //
	if (bIsGameOverActive == false)
	{
		// Creating Widget and adding to viewport //
		GameOverMenu = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
		GameOverMenu->AddToViewport();

		// Get Reference to the Player Controller //
		APlayerController* PlayerController = GetFirstLocalPlayerController();

		// Set up input parameters to the SetupInputMode Function //
		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(GameOverMenu->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		// Set Input Mode //
		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetPause(true);

		// Set the Boolean to "True" to prevent multiple Game Over Menus from happening //
		bIsGameOverActive = true;
	}
}

void UUMGGameInstance::ShowLevelComplete()
{
	if (bIsLevelCompleteActive == false)
	{
		// Creating Widget and adding to viewport //
		LevelComplete = CreateWidget<UUserWidget>(this, LevelCompleteWidgetClass);
		LevelComplete->AddToViewport();

		// Get Reference to the Player Controller //
		APlayerController* PlayerController = GetFirstLocalPlayerController();

		// Set up input parameters to the SetupInputMode Function //
		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(LevelComplete->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		// Set Input Mode  and pause Game//
		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetPause(true);

		// Set the Boolean to "True" to prevent multiple Game Over Menus from happening //
		bIsLevelCompleteActive = true;
	}
}

void UUMGGameInstance::ShowOptions()
{
	OptionsMenu = CreateWidget<UUserWidget>(this, OptionsWidgetClass);
	OptionsMenu->AddToViewport();

	APlayerController* PlayerController = GetFirstLocalPlayerController();

	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(OptionsMenu->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;
}

void UUMGGameInstance::ShowStarting2DCutscene()
{
	// Creating Widget and adding to viewport //
	Starting2DCutscene = CreateWidget<UUserWidget>( this, Starting2DCutsceneWidgetClass );
	Starting2DCutscene->AddToViewport();

	// Get Reference to the Player Controller //
	APlayerController* PlayerController = GetFirstLocalPlayerController();

	// Set up input parameters to the SetupInputMode Function //
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus( Starting2DCutscene->TakeWidget() );
	InputModeData.SetLockMouseToViewportBehavior( EMouseLockMode::DoNotLock );

	// Set Input Mode //
	PlayerController->SetInputMode( InputModeData );
	PlayerController->bShowMouseCursor = true;
}

// These are the functions to remove the various menus //

void UUMGGameInstance::RemoveMenu()
{
	// If Statement to check if the widget is in the viewport //
	if (MainMenu->IsInViewport())
	{
		// Remove the widget from the viewport //
		MainMenu->RemoveFromViewport();

		// Get Reference to the Player Controller //
		APlayerController* PlayerController = GetFirstLocalPlayerController();

		// Set up input parameters to the SetInputMode Function //
		FInputModeGameOnly InputModeData;
		
		// Set Input Mode //
		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = false;
	}
}

void UUMGGameInstance::RemovePause()
{
	// If Statement to check if the widget is in the viewport //
	if (PauseMenu->IsInViewport())
	{
		// Remove the widget from the viewport //
		PauseMenu->RemoveFromViewport();

		// Get Reference to the Player Controller //
		APlayerController* PlayerController = GetFirstLocalPlayerController();

		// Set up input parameters to the SetInputMode Function //
		FInputModeGameOnly InputModeData;

		// Set Input Mode and unpause the Game //
		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetPause(false);
	}
}

void UUMGGameInstance::RemoveGameOver()
{
	// If Statement to check if the widget is in the viewport //
	if (GameOverMenu->IsInViewport())
	{
		// Remove the widget from the viewport //
		GameOverMenu->RemoveFromViewport();

		// Get Reference to the Player Controller //
		APlayerController* PlayerController = GetFirstLocalPlayerController();

		// Set up input parameters to the SetInputMode Function //
		FInputModeGameOnly InputModeData;

		// Set Input Mode //
		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetPause(false);

		// Reset the Boolean to "False" to allow the "GameOver" menu to reappear //
		bIsGameOverActive = false;
	}
}

void UUMGGameInstance::RemoveLevelComplete()
{
	// If Statement to check if the widget is in the viewport //
	if (LevelComplete->IsInViewport())
	{
		// Remove the widget from the viewport //
		LevelComplete->RemoveFromViewport();

		// Get Reference to the Player Controller //
		APlayerController* PlayerController = GetFirstLocalPlayerController();

		// Set up input parameters to the SetInputMode Function //
		FInputModeGameOnly InputModeData;

		// Set Input Mode //
		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetPause(false);

		// Reset the Boolean to "False" to allow the "GameOver" menu to reappear //
		bIsLevelCompleteActive = false;
	}
}

void UUMGGameInstance::RemoveOptions()
{
	if (OptionsMenu->IsInViewport())
	{
		OptionsMenu->RemoveFromViewport();

		APlayerController* PlayerController = GetFirstLocalPlayerController();

		FInputModeGameOnly InputModeData;

		PlayerController->SetInputMode(InputModeData);
		PlayerController->bShowMouseCursor = false;
	}
}

void UUMGGameInstance::RemoveStarting2DCutscene()
{
	// If Statement to check if the widget is in the viewport //
	if (Starting2DCutscene->IsInViewport())
	{
		// Remove the widget from the viewport //
		Starting2DCutscene->RemoveFromViewport();

		// Get Reference to the Player Controller //
		APlayerController* PlayerController = GetFirstLocalPlayerController();

		// Set up input parameters to the SetInputMode Function //
		FInputModeGameOnly InputModeData;

		// Set Input Mode //
		PlayerController->SetInputMode( InputModeData );
		PlayerController->bShowMouseCursor = false;
	}
}

void UUMGGameInstance::QuitGame()
{
	// Get Reference to the Player Controller a//
	APlayerController* PlayerController = GetFirstLocalPlayerController();

	// Using the Kismet Library function we quit the game, using the World object, our reference to the Player Controller and the Enum "EQuitPreference" //
	// We also make sure to ignore any special platform restrictions to quit the game //
	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
}

void UUMGGameInstance::SetMusicToPlay()
{
	if (!BGMusicAmbience && !BGMusicCombat && !BGMusicChase)
	{
		BGMusicAmbience = UGameplayStatics::CreateSound2D(this, AmbienceMusic, 1.0f, 1.0f, 0.0f, nullptr, true, false);
		BGMusicCombat = UGameplayStatics::CreateSound2D(this, CombatMusic, 1.0f, 1.0f, 0.0f, nullptr, true, false);
		BGMusicChase = UGameplayStatics::CreateSound2D(this, ChaseMusic, 1.0f, 1.0f, 0.0f, nullptr, true, false);
	}
}

void UUMGGameInstance::PlayNewMusic( EMusic eNewMusic, float VolumeToFade )
{
	// Declare pointer to new audio component we'll be playing
	UAudioComponent* pNewAudio = nullptr;

	// Find new audio to play with eNewMusic
	switch( eNewMusic )
	{
		case EMusic::Background:
		{
			pNewAudio = BGMusicAmbience;
		}
		break;
		case EMusic::Combat:
		{
			pNewAudio = BGMusicCombat;
		}
		break;
		case EMusic::Chase:
		{
			pNewAudio = BGMusicChase;
		}
		break;
	}

	// If there is audio currently playing
	if( m_pCurrentAudio )
	{
		// Fade out current audio
		m_pCurrentAudio->FadeOut( FadeDuration, 0.0f );

	}
	// If we have valid new audio
	if( pNewAudio )
	{
		// Fade in new audio
		pNewAudio->FadeIn( FadeDuration, VolumeToFade );

		// Update current audio pointer to be the new audio
		m_pCurrentAudio = pNewAudio;
	}
}

void UUMGGameInstance::FadeToSilence()
{
	if (m_pCurrentAudio)
	{
		// Fade out current audio
		m_pCurrentAudio->FadeOut(FadeDuration, 0.0f);
	}
}

void UUMGGameInstance::QuietenMusic()
{
	if (m_pCurrentAudio && m_pCurrentVA->IsPlaying())
	{
		m_pCurrentAudio->SetVolumeMultiplier(0.5f);
	}
}

void UUMGGameInstance::LoudenMusic()
{
	if (m_pCurrentAudio && m_pCurrentVA->GetPlayState() == EAudioComponentPlayState::Stopped)
	{
		m_pCurrentAudio->SetVolumeMultiplier(1.0f);
	}
}

void UUMGGameInstance::SetMusicVolume(float NewVolume)
{
	BGMusicAmbience->SetVolumeMultiplier(NewVolume);
	BGMusicCombat->SetVolumeMultiplier(NewVolume);
	BGMusicChase->SetVolumeMultiplier(NewVolume);
}

void UUMGGameInstance::SetSFXVolume(float NewVolume)
{
	SFXVolumeToSet = NewVolume;
}

void UUMGGameInstance::SetDialogueVolume(float NewVolume)
{
	VACutscene1->SetVolumeMultiplier(NewVolume);
	VAStartLevel->SetVolumeMultiplier(NewVolume);
	VAEndWave1->SetVolumeMultiplier(NewVolume);
	VAEndWave2->SetVolumeMultiplier(NewVolume);
	VAEndWave3->SetVolumeMultiplier(NewVolume);
	VAEndWave4->SetVolumeMultiplier(NewVolume);
	VAEndWave5->SetVolumeMultiplier(NewVolume);
	VAEndWave6->SetVolumeMultiplier(NewVolume);
}

void UUMGGameInstance::SetFieldOfView(float SliderValue)
{
	NewFOV = SliderValue;
}

// -----

void UUMGGameInstance::SetVAToPlay()
{
	if (!VACutscene1 && !VAStartLevel && !VAEndWave1 && !VAEndWave2 && !VAEndWave3 && !VAEndWave4 && !VAEndWave5 && !VAEndWave6 )
	{
		VACutscene1		= UGameplayStatics::CreateSound2D( this, GetVASFX( 0 ), 1.0f, 1.0f, 0.0f, nullptr, true, false );
		VAStartLevel	= UGameplayStatics::CreateSound2D( this, GetVASFX( 1 ), 1.0f, 1.0f, 0.0f, nullptr, true, false );
		VAEndWave1		= UGameplayStatics::CreateSound2D( this, GetVASFX( 2 ), 1.0f, 1.0f, 0.0f, nullptr, true, false );
		VAEndWave2		= UGameplayStatics::CreateSound2D( this, GetVASFX( 3 ), 1.0f, 1.0f, 0.0f, nullptr, true, false );
		VAEndWave3		= UGameplayStatics::CreateSound2D( this, GetVASFX( 4 ), 1.0f, 1.0f, 0.0f, nullptr, true, false );
		VAEndWave4		= UGameplayStatics::CreateSound2D( this, GetVASFX( 5 ), 1.0f, 1.0f, 0.0f, nullptr, true, false );
		VAEndWave5		= UGameplayStatics::CreateSound2D( this, GetVASFX( 6 ), 1.0f, 1.0f, 0.0f, nullptr, true, false );
		VAEndWave6		= UGameplayStatics::CreateSound2D( this, GetVASFX( 7 ), 1.0f, 1.0f, 0.0f, nullptr, true, false );
	}
}

USoundWave* UUMGGameInstance::GetVASFX( int iVAToPlay )
{
	if (m_pVADialogueScript)
	{
		// Use GetRowNames() to store in array to get length of the data table
		TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();

		FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[iVAToPlay] );

		USoundWave* pVASound = pVADialogue->SFX;

		return pVASound;
	}

	return nullptr;
}

FString UUMGGameInstance::GetVASubtitles( int iVASubtitles )
{
	if (m_pVADialogueScript)
	{
		// Use GetRowNames() to store in array to get length of the data table
		TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();

		if (iVASubtitles >= 0 || iVASubtitles <= aVADialogueOptions.Num() - 1)
		{
			FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[iVASubtitles] );

			FString pVSubtitles = pVADialogue->Subtitles[0];

			return pVSubtitles;
		}
	}

	FString noSubs;	// = "No subs available >.<";
	return noSubs;
}

void UUMGGameInstance::SetSubtitles( int iVASubtitlesIndex )
{

	if (m_pVADialogueScript)
	{
		// Use GetRowNames() to store in array to get length of the data table
		TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();

		FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[iVASubtitlesIndex] );

		FString pVSubtitles = pVADialogue->Subtitles[0];

		m_subtitlesText = pVSubtitles;
	}
}

FVADialogue* UUMGGameInstance::GetDialogue( UDataTable* VADialogueTable, FName RowName )
{
	if (!VADialogueTable)
	{
		return nullptr;
	}

	FString ContextString;
	return VADialogueTable->FindRow<FVADialogue>( RowName, ContextString );
}

void UUMGGameInstance::PlayCutsceneVA()
{
	UAudioComponent* pNewVA = nullptr;

	pNewVA = VACutscene1;
	SetSubtitles( 0 );

	TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();
	FVADialogue* pCutsceneDialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[0] );

	CutsceneTimeToNextSub = (pCutsceneDialogue->AssociatedTime / 2) + 1;

	GetWorld()->GetTimerManager().SetTimer( CutsceneSubTimerHandle, this, &UUMGGameInstance::SetNewCutsceneSubtitle, CutsceneTimeToNextSub, false );

	if (pNewVA)
	{
		pNewVA->FadeIn( 0.5f, 1.0f );

		m_pCurrentCutsceneVA = pNewVA;
	}
}

void UUMGGameInstance::PlayNextVA()
{
	m_iVADialogueScriptCount++;

	UAudioComponent* pNewVA = nullptr;

	switch (m_iVADialogueScriptCount)
	{
		// Start of Level Encounter
		case 1:
		{			
			pNewVA = VAStartLevel;
			SetSubtitles( 1 );

			TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();
			FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[1] );

					
			VATimeToNextSub = (pVADialogue->AssociatedTime / 2) - 3;			
			GetWorld()->GetTimerManager().SetTimer( VASubTimerHandle, this, &UUMGGameInstance::SetNewVASubtitle, VATimeToNextSub, false );

			// Clear subtitles after the VA has played
			m_fTimeToClearSub = pVADialogue->AssociatedTime;
			GetWorld()->GetTimerManager().SetTimer( ClearSubTimerHandle, this, &UUMGGameInstance::ClearSubtitles, m_fTimeToClearSub, false);

		}
		break;

		// End of Wave 1 of Encounter
		case 2:
		{
			m_bCanSkipVA = true;

			pNewVA = VAEndWave1;
			SetSubtitles( 2 );

			TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();
			FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[2] );

			m_fTimeToClearSub = pVADialogue->AssociatedTime;
			GetWorld()->GetTimerManager().SetTimer( ClearSubTimerHandle, this, &UUMGGameInstance::ClearSubtitles, m_fTimeToClearSub, false );
		}
		break;

		// End of Wave 2 of Encounter
		case 3:
		{
			pNewVA = VAEndWave2;
			SetSubtitles( 3 );

			TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();
			FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[3] ); 
			
			m_fTimeToClearSub = pVADialogue->AssociatedTime;
			GetWorld()->GetTimerManager().SetTimer( ClearSubTimerHandle, this, &UUMGGameInstance::ClearSubtitles, m_fTimeToClearSub, false );
		}
		break;

		// End of Wave 3 of Encounter
		case 4:
		{
			pNewVA = VAEndWave3;
			SetSubtitles( 4 );

			TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();
			FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[4] );

			m_fTimeToClearSub = pVADialogue->AssociatedTime;
			GetWorld()->GetTimerManager().SetTimer( ClearSubTimerHandle, this, &UUMGGameInstance::ClearSubtitles, m_fTimeToClearSub, false );
		}
		break;

		// End of Wave 4 of Encounter
		case 5:
		{
			pNewVA = VAEndWave4;
			SetSubtitles( 5 );

			TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();
			FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[5] );

			m_fTimeToClearSub = pVADialogue->AssociatedTime;
			GetWorld()->GetTimerManager().SetTimer( ClearSubTimerHandle, this, &UUMGGameInstance::ClearSubtitles, m_fTimeToClearSub, false );
		}
		break;

		// End of Wave 5 of Encounter
		case 6:
		{
			pNewVA = VAEndWave5;
			SetSubtitles( 6 );

			TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();
			FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[6] );

			m_fTimeToClearSub = pVADialogue->AssociatedTime;
			GetWorld()->GetTimerManager().SetTimer( ClearSubTimerHandle, this, &UUMGGameInstance::ClearSubtitles, m_fTimeToClearSub, false );
		}
		break;

		// End of Wave 6 of Encounter
		case 7:
		{
			pNewVA = VAEndWave6;
			SetSubtitles( 7 );

			TArray<FName> aVADialogueOptions = m_pVADialogueScript->GetRowNames();
			FVADialogue* pVADialogue = GetDialogue( m_pVADialogueScript, aVADialogueOptions[7] );

			m_fTimeToClearSub = pVADialogue->AssociatedTime;
			GetWorld()->GetTimerManager().SetTimer( ClearSubTimerHandle, this, &UUMGGameInstance::ClearSubtitles, m_fTimeToClearSub, false );
		}
		break;

		default:
		{
			GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Orange, TEXT( "VA nothing to play" ) );
		}
		break;
	}


	if (m_pCurrentVA)
	{
		m_pCurrentVA->Stop();
	}

	if (pNewVA)
	{
		pNewVA->Play();

		m_pCurrentVA = pNewVA;
	}

}

void UUMGGameInstance::SetNewVASubtitle()
{
	if (m_bCanSkipVA == false)
	{
		m_iVASubtitlesCount++;

		TArray<FName> pVACutsceneDialogueOptions = m_pVADialogueScript->GetRowNames();

		FVADialogue* pCutsceneDialogue = GetDialogue( m_pVADialogueScript, pVACutsceneDialogueOptions[m_iVADialogueScriptCount] );

		if (pCutsceneDialogue != nullptr && m_iVASubtitlesCount <= pCutsceneDialogue->Subtitles.Num())
		{
			m_subtitlesText = pCutsceneDialogue->Subtitles[m_iVASubtitlesCount];
		}

		AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );
		if (HUDWidgetRef != nullptr)
		{
			HUDWidgetRef->GetHUDWidgetRef()->UpdateSubtitlesText( GetSubtitlesText() );
		}

		GetWorld()->GetTimerManager().ClearTimer( VASubTimerHandle );
	}
}


void UUMGGameInstance::SetNewCutsceneSubtitle()
{
	if (m_bCanSkipCutsceneVA == false)
	{
		m_iVASubtitlesCount++;

		TArray<FName> pVACutsceneDialogueOptions = m_pVADialogueScript->GetRowNames();

		FVADialogue* pCutsceneDialogue = GetDialogue( m_pVADialogueScript, pVACutsceneDialogueOptions[m_iVADialogueScriptCount] );

		m_subtitlesText = pCutsceneDialogue->Subtitles[m_iVASubtitlesCount];

		UUMG2DCutscene* CutsceneWidgetRef = Cast<UUMG2DCutscene>( GetStarting2DCutscene() );
		if (CutsceneWidgetRef != nullptr)
		{
			CutsceneWidgetRef->UpdateSubtitlesText( GetSubtitlesText() );
		}
	}
}

void UUMGGameInstance::StopCutsceneVADialogue()
{		
	if (m_pCurrentCutsceneVA)
	{
		m_bCanSkipCutsceneVA = true;

		m_pCurrentCutsceneVA->FadeOut( 0.3f, 0.0f );
	}
}

void UUMGGameInstance::SetSubtitlesText( UTextBlock* SubText )
{
	SubText->SetText( FText::FromString( m_subtitlesText ) );
}

void UUMGGameInstance::ClearSubtitles()
{	
	AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );
	if (HUDWidgetRef != nullptr)
	{
		HUDWidgetRef->GetHUDWidgetRef()->ClearSubtitlesText();
	}

	GetWorld()->GetTimerManager().ClearTimer( ClearSubTimerHandle );
}

void UUMGGameInstance::GameOverResetVA()
{
	m_iVADialogueScriptCount = 0;
	m_iVASubtitlesCount = 0;
	m_bCanSkipVA = false;
}

void UUMGGameInstance::MainMenuRestartVA()
{
	m_iVADialogueScriptCount = -1;
	m_iVASubtitlesCount = 0;
	m_bCanSkipVA = false;
}

void UUMGGameInstance::AudioFinished()
{
	GEngine->AddOnScreenDebugMessage( -1, 5.0f, FColor::Orange, TEXT( "OnAudioFinishedCalled" ) );

	AGCFPSHUD* HUDWidgetRef = Cast<AGCFPSHUD>( GetWorld()->GetFirstPlayerController()->GetHUD() );
	if (HUDWidgetRef != nullptr)
	{
		HUDWidgetRef->GetHUDWidgetRef()->ClearSubtitlesText();
	}
}

void UUMGGameInstance::ClearCutsceneTimer()
{
	GetWorld()->GetTimerManager().ClearTimer( CutsceneSubTimerHandle );
}

void UUMGGameInstance::ResetSubtitlesCount()
{
	m_iVASubtitlesCount = 0;
}

void UUMGGameInstance::ClearSubtitlesText()
{
	m_subtitlesText = "";
}

void UUMGGameInstance::StopVA()
{
	if (m_pCurrentVA)
	{
		m_pCurrentVA->FadeOut( 0.3f, 0.0f );
	}
}
