// ==================================================== //
//	Gamer Camp 2020-21 -> Revolt						//
// ==================================================== //
//	Contributors:										//
//		Dan Price										//
//		Henrique Teixeira								//
//		Puia Catalin									//
//		Logan Lai										//
//		Umeer Rama										//
//		Ysabela Bathan									//
// ==================================================== //

#include "FPCharacter.h"
#include "../GCFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GCFPS/Player/Components/Movement/FPCharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GCFPS/Weapons/BaseWeapon.h"
#include "GCFPS/Interactables/ButtonInteractable.h"

#include "PhysicalMaterials/PhysicalMaterial.h"

// Camera Component
#include "GCFPS/Player/Camera/FPCameraComponent.h"

// Movement Components
#include "GCFPS/Player/Components/Movement/SlideComponent.h"
#include "GCFPS/Player/Components/Movement/VaultingComponent.h"
#include "GCFPS/Player/Components/Movement/WallRunComponent.h"

// Melee Component
#include "GCFPS/Player/Components/Melee/MeleeComponent.h"

// AI Stimulus
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
//#include "Perception/AISense_Hearing.h"

#include "Camera/FPPlayerCameraManager.h"
#include "GCFPS/Player/Components/Stats/PlayerRegenStatsComponent.h"
#include "GCFPS/Player/Components/Stats/PickUpComponent.h"
#include "GCFPS/Player/Components/Stats/ObjectivesComponent.h"

#include "GCFPS/Menu/UMG/UMGGameInstance.h"
#include "GCFPS/Player/Components/AI/GhostComponent.h"
#include "GCFPS/Player/Components/Cheats/CheatComponent.h"

// AnimInstance
#include "Animation/AnimInstance/PlayerAnimInstance.h"

DEFINE_LOG_CATEGORY_STATIC( LogFPChar, Warning, All );

//////////////////////////////////////////////////////////////////////////
// AFPCharacter

AFPCharacter::AFPCharacter( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer.SetDefaultSubobjectClass<UFPCharacterMovementComponent>( ACharacter::CharacterMovementComponentName ) )
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize( 55.f, 96.0f );
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AFPCharacter::OnCapsuleComponentHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFPCharacter::OnOverLap);

	// set our turn rates for input
	BaseTurnRate = 75.f;
	BaseLookUpRate = 75.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UFPCameraComponent>( TEXT( "FirstPersonCamera" ) );
	FirstPersonCameraComponent->SetupAttachment( GetCapsuleComponent() );
	FirstPersonCameraComponent->SetRelativeLocation( FVector( -39.56f, 1.75f, 64.f ) ); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "CharacterMesh1P" ) );
	Mesh1P->SetOnlyOwnerSee( true );
	Mesh1P->SetupAttachment( FirstPersonCameraComponent );
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation( FRotator( 1.9f, -19.19f, 5.2f ) );
	Mesh1P->SetRelativeLocation( FVector( -0.5f, -4.4f, -155.7f ) );

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>( TEXT( "FP_Gun" ) );
	FP_Gun->SetOnlyOwnerSee( false );			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment( RootComponent );

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>( TEXT( "MuzzleLocation" ) );
	FP_MuzzleLocation->SetupAttachment( FP_Gun );
	FP_MuzzleLocation->SetRelativeLocation( FVector( 0.2f, 48.4f, -10.6f ) );

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector( 100.0f, 0.0f, 10.0f );

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P and FP_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.
	// 
	// Initialize Vaulting Component
	m_pACVaultingComponent = CreateDefaultSubobject<UVaultingComponent>(TEXT("Vaulting Component"));

	// Initialize Melee Component
	m_pACMeleeComponent = CreateDefaultSubobject<UMeleeComponent>(TEXT("Melee Component"));

	// Initialize Crouch and Slide Component
	m_pACSlideComponent = CreateDefaultSubobject<USlideComponent>( TEXT( "Crouch and Slide Component" ) );

	// Initialize Wall Run Component
	m_pACWallRunComponent = CreateDefaultSubobject<UWallRunComponent>( TEXT( "Wall Run Component" ) );

	//Initialize Stats Component
	PlayerStats = CreateDefaultSubobject<UPlayerRegenStatsComponent>(TEXT("Stats"));

	//Initialize PickUp Component
	PickUps = CreateDefaultSubobject<UPickUpComponent>(TEXT("PickUp"));

	//Initialize Objectives Component
	Objectives = CreateDefaultSubobject<UObjectivesComponent>(TEXT("Objectives"));

	// Initialize Ghost Component
	m_pACGhostComponent = CreateDefaultSubobject<UGhostComponent>( TEXT( "Ghost Component" ) );

	// Initialize Cheats Component
	m_pACCheatsComponent = CreateDefaultSubobject<UCheatComponent>( TEXT("Cheats Component") );

	WeaponIndex = 0;
	// Set jump count to 2 for double jump
	JumpMaxCount = 2;

	//f_ShootDelay = 100.0f;

	ButtonInteract = nullptr;

	SetupStimulus();

	m_ePlayerActionState = EPlayerActionState::NoAction;
}

void AFPCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Get Henrique's custom character movement component, save it as a pointer
	// This will refrain us from having to downcast from GetCharacterMovement() every time we need to use it.
	UFPCharacterMovementComponent* pCharMovementComponent = Cast<UFPCharacterMovementComponent>( GetCharacterMovement() );
	if( pCharMovementComponent )
	{
		m_ACFPCharacterMovementComponent = pCharMovementComponent;
	}
	
	APlayerController* pPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(pPlayerController)
	{
		pPlayerController->PlayerCameraManagerClass = AFPPlayerCameraManager::StaticClass();
	}
}

void AFPCharacter::CheckJumpInput( float DeltaTime )
{
	// This function is exactly the same as the ACharacter::CheckJumpInput(),
	// Except it's got a quality of life improvement:
	// The player is able to do a first jump after walking off a ledge or coming off the wall
	// This is possible through our check m_ACFPCharacterMovementComponent->CanStillDoFirstJumpWhileFalling() a few lines below
	if( m_ACFPCharacterMovementComponent->CheckCanJump() )
	{
		JumpCurrentCountPreJump = JumpCurrentCount;

		if( GetCharacterMovement() )
		{
			if( bPressedJump )
			{
				// If this is the first jump and we're already falling,
				// We want to check if we're still able to jump a first time if we just started falling
				// This just makes life easier for the player
				// If we're past that point, we add to the JumpCurrentCount and effectively skip the first jump
				const bool bFirstJump = JumpCurrentCount == 0;
				if( bFirstJump && GetCharacterMovement()->IsFalling() && !m_ACFPCharacterMovementComponent->CanStillDoFirstJumpWhileFalling())
				{
					JumpCurrentCount++;
				}

				const bool bDidJump = CanJump() && GetCharacterMovement()->DoJump( bClientUpdating );
				if( bDidJump )
				{
					// Transition from not (actively) jumping to jumping.
					if( !bWasJumping )
					{
						JumpCurrentCount++;
						JumpForceTimeRemaining = GetJumpMaxHoldTime();
						OnJumped();
					}
				}
				bWasJumping = bDidJump;
			}
		}
	}
}

void AFPCharacter::Landed( const FHitResult& Hit )
{
	// Call Super::Landed for the generic character movement component updates
	Super::Landed( Hit );
	// Call FPCharacterMovementComponent's OnLanded() which will determine the new movement mode
	if( m_ACFPCharacterMovementComponent )
	{
		m_ACFPCharacterMovementComponent->OnLanded( Hit );
	}
}

void AFPCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(GetGameInstance());

	if (GameInstance)
	{
		FirstPersonCameraComponent->SetFieldOfView(GameInstance->GetFOVValue());
	}

	//setCurrentWeapon(GetWorld()->SpawnActor<ABaseWeapon>(ShotGunClass));

	CurrentWeapon = GetWorld()->SpawnActor<ABaseWeapon>(ShotGunClass);//setting the shotgun to be the starting weapon
	
	//Adding each of the weapons to the weapon array, setting the visibility to false at the start , setting the owner of the weapons to be the player
	//attaching the weapons to the player hand socket
	if (CurrentWeapon)
	{
		WeaponArray.Add(CurrentWeapon);
		CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("ShotgunGripPoint"));

		CurrentWeapon->SetOwner(this);
		

	}
	if(ABaseWeapon* AssaultRifle = GetWorld()->SpawnActor<ABaseWeapon>(AssaultRifleClass))
	{
		//Weapon->GetSkeletalMesh()->SetHiddenInGame(true);
		AssaultRifle->GetSkeletalMesh()->SetVisibility(false);
		AssaultRifle->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("AssaultRifleGripPoint"));
		AssaultRifle->SetOwner(this);
		WeaponArray.Add(AssaultRifle);
		m_eCurrentWeaponType = EWeaponType::AssaultRifle;
	}
	if(ABaseWeapon* GrenadeLauncher = GetWorld()->SpawnActor<ABaseWeapon>(GrenadeLauncherClass))
	{
		//Weapon->GetSkeletalMesh()->SetHiddenInGame(true);
		GrenadeLauncher->GetSkeletalMesh()->SetVisibility(false);
		GrenadeLauncher->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
		GrenadeLauncher->SetOwner(this);
		WeaponArray.Add(GrenadeLauncher);
		
	}
	if(ABaseWeapon* FlarePistol = GetWorld()->SpawnActor<ABaseWeapon>(FlarePistolClass))
	{

		FlarePistol->GetSkeletalMesh()->SetVisibility(false);
		FlarePistol->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("PistolGripPoint"));
		FlarePistol->SetOwner(this);
		WeaponArray.Add(FlarePistol);
		m_eCurrentWeaponType = EWeaponType::IncendiaryPistol;
	}
		
	if (fOnWeaponChanged.IsBound())
	{
		fOnWeaponChanged.Broadcast(EWeaponType::Shotgun);
	}


	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame( false, true );

	if (Mesh1P && Mesh1P->GetAnimInstance())
	{
		m_pAIPlayerAnimInstance = Mesh1P->GetAnimInstance();
	}
}

void AFPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult Hit;
	FVector Start = FirstPersonCameraComponent->GetComponentLocation();

	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector End = (ForwardVector * 200.f) + Start;
	FCollisionQueryParams CollisionParams;

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams))
	{
		if (Hit.bBlockingHit)
		{
			if (Hit.GetActor() && Hit.GetActor()->GetClass()->IsChildOf(AButtonInteractable::StaticClass()))
			{
				ButtonInteract = Cast<AButtonInteractable>(Hit.GetActor());
			}
		}
	}
	else
	{
		ButtonInteract = nullptr;
	}

	AutomaticVaulting();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFPCharacter::SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent )
{
	// Set up gameplay key bindings
	check( PlayerInputComponent );

	// Bind jump events
	PlayerInputComponent->BindAction( "Jump", IE_Pressed, this, &AFPCharacter::OnJump );
	PlayerInputComponent->BindAction( "Jump", IE_Released, this, &ACharacter::StopJumping );

	// Bind fire event
	PlayerInputComponent->BindAction( "Fire", IE_Pressed, this, &AFPCharacter::OnFire );
	PlayerInputComponent->BindAction( "Fire", IE_Released, this, &AFPCharacter::StopFire);

	// Bind Interact
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPCharacter::OnInteract);

	// Bind Weapon swap
	PlayerInputComponent->BindAction("SwapNextWeapon", IE_Pressed, this, &AFPCharacter::SwapNextWeapon);
	PlayerInputComponent->BindAction("SwapPreviousWeapon", IE_Pressed, this, &AFPCharacter::SwapPreviousWeapon);
	PlayerInputComponent->BindAction("PrimaryWeapon", IE_Pressed, this, &AFPCharacter::SwitchPrimary);
	PlayerInputComponent->BindAction("OtherPrimaryWeapon", IE_Pressed, this, &AFPCharacter::SwitchOtherPrimary);
	PlayerInputComponent->BindAction("SecondaryWeapon", IE_Pressed, this, &AFPCharacter::SwitchSecondary);

	// Bind Weapon Reload
	PlayerInputComponent->BindAction("ReloadWeapon", IE_Pressed, this, &AFPCharacter::ReloadWeapon);
	
	// Bind Movement events
	PlayerInputComponent->BindAxis( "MoveForward", this, &AFPCharacter::MoveForward );
	PlayerInputComponent->BindAxis( "MoveRight", this, &AFPCharacter::MoveRight );

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis( "Turn", this, &APawn::AddControllerYawInput );
	PlayerInputComponent->BindAxis( "TurnRate", this, &AFPCharacter::TurnAtRate );
	PlayerInputComponent->BindAxis( "LookUp", this, &APawn::AddControllerPitchInput );
	PlayerInputComponent->BindAxis( "LookUpRate", this, &AFPCharacter::LookUpAtRate );

	// Bind Melee Event
	PlayerInputComponent->BindAction("Melee", IE_Pressed, this, &AFPCharacter::OnMeleeAttack);

	// Bind Sprint Events
	PlayerInputComponent->BindAction( "Sprint", IE_Pressed, this, &AFPCharacter::PressedSprintButton );
	PlayerInputComponent->BindAction( "Sprint", IE_Released, this, &AFPCharacter::ReleasedSprintButton );

	// Bind Crouch Events
	PlayerInputComponent->BindAction( "Crouch", IE_Pressed, this, &AFPCharacter::PressedCrouchButton );
	PlayerInputComponent->BindAction( "Crouch", IE_Released, this, &AFPCharacter::ReleasedCrouchButton );
	
	// Bind PauseGame Event
	PlayerInputComponent->BindAction( "PauseGame", IE_Pressed, this, &AFPCharacter::ShowPauseMenu );

	// Bind Cheats
	PlayerInputComponent->BindAction( "AnyKey", IE_Pressed, m_pACCheatsComponent, &UCheatComponent::OnKeyPressed );

}

void AFPCharacter::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		PickUps->OnOverLap(OtherActor);
	}
}

void AFPCharacter::SwapNextWeapon()
{
	//increment the weapon array settting the next weapon to be the current weapon
	switch (m_ePlayerActionState)
	{
	case EPlayerActionState::NoAction:
	case EPlayerActionState::Shooting:
	{
		if (CurrentWeapon)
		{
			if (WeaponArray.Num() > WeaponIndex + 1)
			{
				++WeaponIndex;
				InitiatePlayerActionStateSwitch(EPlayerActionState::SwitchWeapon);
			}
			else
			{
				WeaponIndex = 0;
				InitiatePlayerActionStateSwitch(EPlayerActionState::SwitchWeapon);
			}
		}
		break;
	}
	default: 
	{
		break;
	}
	}
}

void AFPCharacter::SwapPreviousWeapon()
{
	//increment the weapon array settting the next weapon to be the current weapon
	switch (m_ePlayerActionState)
	{
	case EPlayerActionState::NoAction:
	case EPlayerActionState::Shooting:
	{
		if (CurrentWeapon)
		{
			if (WeaponIndex - 1 >= 0)
			{
				--WeaponIndex;

				InitiatePlayerActionStateSwitch(EPlayerActionState::SwitchWeapon);

			}
			else
			{
				WeaponIndex = 2;
				InitiatePlayerActionStateSwitch(EPlayerActionState::SwitchWeapon);
			}

		}
	}
	default:
	{
		break;
	}
	}
}

void AFPCharacter::SwitchPrimary()
{
	if (CurrentWeapon)
	{
	    ABaseWeapon* NextWeapon = WeaponArray[0];
	
		InitiatePlayerActionStateSwitch(EPlayerActionState::SwitchWeapon);
	}
}

void AFPCharacter::SwitchOtherPrimary()
{
	if (CurrentWeapon)
	{
		ABaseWeapon* NextWeapon = WeaponArray[1];

		InitiatePlayerActionStateSwitch(EPlayerActionState::SwitchWeapon);
	}
}

void AFPCharacter::SwitchSecondary()
{
	if (CurrentWeapon)
	{
		ABaseWeapon* NextWeapon = WeaponArray[2];

		InitiatePlayerActionStateSwitch(EPlayerActionState::SwitchWeapon);
	}
}

UFPCharacterMovementComponent* AFPCharacter::GetCharacterMovementComponent() const
{
	return m_ACFPCharacterMovementComponent;
}

void AFPCharacter::PressedSprintButton()
{
	// Set flag to true
	m_bHoldingSprintButton = true;
	if( m_ACFPCharacterMovementComponent )
	{
		// Try to sprint through custom movement component
		m_ACFPCharacterMovementComponent->BeginSprintAttempted();
	}
}

void AFPCharacter::ReleasedSprintButton()
{
	// Set flag to false
	m_bHoldingSprintButton = false;
	if( m_ACFPCharacterMovementComponent )
	{
		// Try ending the sprint through custom movement component
		m_ACFPCharacterMovementComponent->StopSprintAttempted();
	}
}

void AFPCharacter::PressedCrouchButton()
{
	// Set flag to true
	m_bHoldingCrouchButton = true;
	if( m_pACSlideComponent )
	{
		// Try crouching through the Slide component
		m_pACSlideComponent->BeginCrouchAttempted();
	}
}

void AFPCharacter::ReleasedCrouchButton()
{
	// Set flag to false
	m_bHoldingCrouchButton = false;
	if( m_pACSlideComponent )
	{
		// Try ending crouch through the Slide component
		m_pACSlideComponent->EndCrouchAttempted();
	}
}

bool AFPCharacter::GetHoldingCrouchButton() const
{
	return m_bHoldingCrouchButton;
}

UWallRunComponent* AFPCharacter::GetWallRunComponent() const
{
	return m_pACWallRunComponent;
}

USlideComponent* AFPCharacter::GetSlideComponent() const
{
	return m_pACSlideComponent;
}

UGhostComponent* AFPCharacter::GetGhostComponent() const
{
	return m_pACGhostComponent;
}

UPlayerRegenStatsComponent* AFPCharacter::GetRegenComponent() const
{
	return PlayerStats;
}

bool AFPCharacter::GetHoldingSprintButton() const
{
	return m_bHoldingSprintButton;
}

void AFPCharacter::OnFire()
{
	switch (m_ePlayerActionState)
	{
		// can only shoot weapon if no other action is being performed
		case EPlayerActionState::NoAction :
		{
			InitiatePlayerActionStateSwitch(EPlayerActionState::Shooting);
			break;
		}
		default:
		{
			break;
		}
	}
}

void AFPCharacter::StopFire()
{
	switch (m_ePlayerActionState)
	{
		case EPlayerActionState::Shooting :
		{
			InitiatePlayerActionStateSwitch(EPlayerActionState::NoAction);
			break;
		}
		default:
		{
			break;
		}
	}
}

void AFPCharacter::ReloadWeapon()
{
	CurrentWeapon->ReloadWeapon();//not in use
}

void AFPCharacter::MoveForward( float Value )
{
	m_fMoveForward = Value;

	if(m_ACFPCharacterMovementComponent)
	{
		m_ACFPCharacterMovementComponent->SetMoveForwardInputAxis(Value);
	}
	
	if( Value != 0.0f )
	{
		// add movement in that direction
		if( m_ACFPCharacterMovementComponent )
		{
			if( m_ACFPCharacterMovementComponent->GetCustomMovementState() != EFPMovementState::WallRunning )
			{
				AddMovementInput( GetActorForwardVector(), Value );
			}
		}
		else
		{
				AddMovementInput( GetActorForwardVector(), Value );
		}
	}
}

void AFPCharacter::MoveRight( float Value )
{
	if (m_ACFPCharacterMovementComponent)
	{
		m_ACFPCharacterMovementComponent->SetMoveRightInputAxis(Value);
	}
	
	if( Value != 0.0f )
	{

		// add movement in that direction
		if( m_ACFPCharacterMovementComponent )
		{
			if( m_ACFPCharacterMovementComponent->GetCustomMovementState() != EFPMovementState::WallRunning )
			{
				AddMovementInput( GetActorRightVector(), Value );
			}
		}
		else
		{
			AddMovementInput( GetActorRightVector(), Value );
		}
	}
}

void AFPCharacter::TurnAtRate( float Rate )
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput( Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() );
}

void AFPCharacter::LookUpAtRate( float Rate )
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput( Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() );
}

void AFPCharacter::OnJump()
{
	ACharacter::Jump();
}

void AFPCharacter::OnMeleeAttack()
{
	switch (m_ePlayerActionState)
	{
	case EPlayerActionState::NoAction:
	case EPlayerActionState::Shooting:
	case EPlayerActionState::SwitchWeapon:
		InitiatePlayerActionStateSwitch(EPlayerActionState::Melee);
		break;
	default:
		InitiatePlayerActionStateSwitch(EPlayerActionState::NoAction);
		break;
	}
}

void AFPCharacter::OnCapsuleComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// If we have the Wall Run component
	if( m_pACWallRunComponent )
	{
		// If hit info is valid
		if( ( OtherActor != NULL ) && ( OtherActor != this ) && ( OtherComp != NULL ) )
		{
			// If not currently Wall Running
			if( !m_pACWallRunComponent->GetIsWallRunning() )
			{
				// If our custom movement component indicates player is currently in mid-air && 
				// if this hit is an angle at which the Wall that can be used to WallRun &&
				// if this Actor does not have the "NoWallRun" tag &&
				// if the generic character movement component logic indicates the player is currently falling
				if(GetCharacterMovementComponent()->GetCustomMovementState() == EFPMovementState::Jumping && 
				m_pACWallRunComponent->CanSurfaceBeWallRun(Hit.ImpactNormal) &&
				m_pACWallRunComponent->IsSurfaceWallRunEnabled( OtherActor ) &&
				m_pACWallRunComponent->TestTopLineTrace(Hit) &&
				GetCharacterMovementComponent()->IsFalling() )
				{
					// Update Wall Run information (wall normal, run direction, etc)
					GetWallRunComponent()->UpdateWallRunStatus( Hit.ImpactNormal );
					// Transition to Wall Run
					GetCharacterMovementComponent()->InitiateMovementStateSwitch( EFPMovementState::WallRunning );		
				}
			}
		}

	}

}

void AFPCharacter::SetupStimulus()
{
	stimulus = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>( TEXT( "stimulus" ) );
	stimulus->RegisterForSense( TSubclassOf<UAISense_Sight>() );
	stimulus->RegisterWithPerceptionSystem();
}

void AFPCharacter::ShowPauseMenu()
{
	UUMGGameInstance* GameInstance = Cast<UUMGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->ShowPause();
}


void AFPCharacter::OnInteract()
{
	if (ButtonInteract)
	{
		ButtonInteract->OpenDoor();
	}
}

void AFPCharacter::InitiatePlayerActionStateSwitch(EPlayerActionState eNewPlayerActionState)
{
	if (eNewPlayerActionState != m_ePlayerActionState || m_ePreviousPlayerActionState == EPlayerActionState::Shooting)
	{
		OnLeavePlayerActionState(eNewPlayerActionState);
		SwitchPlayerActionState(eNewPlayerActionState);
	}
}

void AFPCharacter::OnLeavePlayerActionState(EPlayerActionState& eNewPlayerActionState)
{
	switch (m_ePlayerActionState)
	{
		case EPlayerActionState::NoAction:
		{
			break;
		}
		case EPlayerActionState::Melee:
		{
			break;
		}
		case EPlayerActionState::Shooting:
		{
			if (CurrentWeapon != nullptr)
			{
				CurrentWeapon->StopFiringWeapon();
			}
			break;
		}
		case EPlayerActionState::SwitchWeapon:
		{
			break;
		}
		case EPlayerActionState::Vault:
		{
			break;
		}
		default:
		{
			break;
		}
	}
}

void AFPCharacter::OnEnterPlayerActionState(EPlayerActionState& eNewPlayerActionState)
{
	switch (eNewPlayerActionState)
	{
		case EPlayerActionState::NoAction:
		{
			break;
		}
		case EPlayerActionState::Melee:
		{
			if (nullptr != m_pACMeleeComponent)
			{
				m_pACMeleeComponent->MeleeAttack();

				if (m_pAIPlayerAnimInstance != nullptr)
				{
					UAnimMontage* MeleeAnimMontage = m_pACMeleeComponent->GetMeleeMontageAnimation();
					if (nullptr != MeleeAnimMontage && !(m_pAIPlayerAnimInstance->Montage_IsPlaying(MeleeAnimMontage)))
					{
						m_pAIPlayerAnimInstance->Montage_Play(MeleeAnimMontage, 2.f);
					}
				}
			}
			break;
		}
		case EPlayerActionState::Shooting:
		{
			if (CurrentWeapon != nullptr)
			{
				CurrentWeapon->FireAutomatic();
			}
			break;
		}
		case EPlayerActionState::SwitchWeapon:
		{
			if (m_pAIPlayerAnimInstance != nullptr)
			{
				float fMontagePlayRate = 1.f;
				m_pAIPlayerAnimInstance->Montage_Play(m_pWeaponSwitchMontage, fMontagePlayRate);
			}
			break;
		}
		case EPlayerActionState::Vault:
		{
			if (m_pACVaultingComponent)
			{
				m_pACVaultingComponent->InitiateVaulting();
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void AFPCharacter::SwitchPlayerActionState(EPlayerActionState& eNewPlayerActionState)
{
	OnEnterPlayerActionState(eNewPlayerActionState);
	m_ePreviousPlayerActionState = m_ePlayerActionState;
	m_ePlayerActionState = eNewPlayerActionState;
}

void AFPCharacter::SwitchWeapons()
{
	//incrementing  the array setting the next weapon to be the current weapon and setting the visibility to true if the next weapon is the current weapon
	if (ABaseWeapon* NextWeapon = WeaponArray[WeaponIndex])
	{
		CurrentWeapon->GetSkeletalMesh()->SetVisibility(false);// ->SetHiddenInGame(true);
		CurrentWeapon->StopFiringWeapon();
		CurrentWeapon = NextWeapon;
		CurrentWeapon->GetSkeletalMesh()->SetVisibility(true);// ->SetHiddenInGame(false);

		if (fOnWeaponChanged.IsBound())
		{
			fOnWeaponChanged.Broadcast(CurrentWeapon->GetCurrentWeaponType());
		}
	}
}

void AFPCharacter::AutomaticVaulting()
{
	// Automatically vault
	if (m_ACFPCharacterMovementComponent != nullptr)
	{
		// check for the current state the movement statemachine is in, specifically for static, walking, sprinting and wallrunning, as we only want to allow vaulting during those movement states
		switch (m_ACFPCharacterMovementComponent->GetCustomMovementState())
		{
		case EFPMovementState::Static:
		case EFPMovementState::Walking:
		case EFPMovementState::Sprinting:
		case EFPMovementState::WallRunning:
		{
			// check for the current state of the Player Action Statemachine for NoAction, since vaulting should only be available to perform when no other action is currently being performed
			switch (m_ePlayerActionState)
			{
			case EPlayerActionState::NoAction:
			{
				// Vaulting should only happen when moving forwards, not while standing still
				if (m_pACVaultingComponent != nullptr && m_fMoveForward > 0.f)
				{
					if (m_pACVaultingComponent->InitiateVaulting())
					{
						InitiatePlayerActionStateSwitch(EPlayerActionState::Vault);
					}
				}
				break;
			}
			default:
				break;
			}
			break;
		}
		default:
			break;
		}
	}
}

void AFPCharacter::ResetActionStateFromSwitchingWeapons()
{
	switch(m_ePreviousPlayerActionState)
	{
	case EPlayerActionState::Shooting:
	{
		InitiatePlayerActionStateSwitch(EPlayerActionState::Shooting);
		break;
	}
	case EPlayerActionState::NoAction:
	case EPlayerActionState::SwitchWeapon:
	case EPlayerActionState::Vault:
	{
		InitiatePlayerActionStateSwitch(EPlayerActionState::NoAction);
		break;
	}
	}
}
