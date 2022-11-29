#include "UMGMenuWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "UMGGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "GCFPS/Cutscenes/UMG2DCutscene.h"


// NativeConstruct is a function that exists within the class we have inherited, which we can use. We will also be assigning functionality //
// to our UI Elements here //
void UUMGMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddUniqueDynamic(this, &UUMGMenuWidget::OnPlayButtonClicked);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UUMGMenuWidget::OnQuitButtonClicked);
	OptionsButton->OnClicked.AddUniqueDynamic(this, &UUMGMenuWidget::OnOptionsButtonClicked);

	EndDelegate.BindDynamic( this, &UUMGMenuWidget::AnimationFinishedCanvasFadeOut );
	BindToAnimationFinished( AnimMainMenuFadeOut, EndDelegate );

}

// These are functions that we will assign to UI Elements //

// This function will be assigned to a button. When the button is clicked, it will run a function that removes the Main Menu widget on screen //
// and changes the current level to the level that is stored in "MapName" //
void UUMGMenuWidget::OnPlayButtonClicked()
{
	PlayAnimation( AnimMainMenuFadeOut );

}

void UUMGMenuWidget::OnOptionsButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->RemoveMenu();
	GameInstance->ShowOptions();
}

// This function will be assigned to a button. When the button is clicked, it will run a function that simply quits the game //
void UUMGMenuWidget::OnQuitButtonClicked()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->QuitGame();

}

void UUMGMenuWidget::AnimationFinishedCanvasFadeOut()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>( UGameplayStatics::GetGameInstance( GetWorld() ) );
	GameInstance->RemoveMenu();
	GameInstance->ShowStarting2DCutscene();
	GameInstance->PlayCutsceneVA();

	//UUMG2DCutscene* CutsceneWidgetRef = Cast<UUMG2DCutscene>( GetWorld() );
	//CutsceneWidgetRef->UpdateSubtitlesText( GameInstance->GetSubtitlesText() );


	UUMG2DCutscene* CutsceneWidgetRef = Cast<UUMG2DCutscene>( GameInstance->GetStarting2DCutscene() );
	if (CutsceneWidgetRef != nullptr)
	{
		CutsceneWidgetRef->UpdateSubtitlesText( GameInstance->GetSubtitlesText() );
	}


}