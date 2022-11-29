#include "UMGPauseMenu.h"
#include "UMGGameInstance.h"
#include "Components/Button.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

// NativeConstruct is a function that exists within the class we have inherited, which we can use. We will also be assigning functionality //
// to our UI Elements here //
void UUMGPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	ResumeButton->OnClicked.AddUniqueDynamic(this, &UUMGPauseMenu::OnResumeButtonClicked);
	RestartButton->OnClicked.AddUniqueDynamic(this, &UUMGPauseMenu::OnRestartButtonClicked);
	ReturnButton->OnClicked.AddUniqueDynamic(this, &UUMGPauseMenu::OnReturnButtonClicked);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UUMGPauseMenu::OnDesktopButtonClicked);

	FadeVolume = 1.0f;
}

// These are functions that we will assign to UI Elements //

// This function will be assigned to a button. When the button is clicked, it will run a function that removes the pause menu widget on screen //
// and it will unpause the game //
void UUMGPauseMenu::OnResumeButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->RemovePause();
}

void UUMGPauseMenu::OnRestartButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->PlayNewMusic(EMusic::Background, FadeVolume);
	GameInstance->RemovePause();

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

// This function will be assigned to a button. When the button is clicked, it will run a function that will remove the pause menu widget on screen //
// and change the current level to the Main Menu level //
void UUMGPauseMenu::OnReturnButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->PlayNewMusic(EMusic::Background, FadeVolume);
	GameInstance->RemovePause();

	UGameplayStatics::OpenLevel(this, "MainMenuMap");
}

void UUMGPauseMenu::OnDesktopButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->QuitGame();
}
