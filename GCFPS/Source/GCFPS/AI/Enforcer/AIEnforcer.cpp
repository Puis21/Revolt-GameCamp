#include "GCFPS/AI/Enforcer/AIEnforcer.h"

#include <BehaviorTree/BlackboardComponent.h>

#include "../Components/Shooting/LineTraceAIShootingComponent.h"
#include "../Components/Shooting/ProjectileAIShootingComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GCFPS/AI/Enforcer/AIEnforcerController.h"
#include "GCFPS/AI/Enforcer/AIEnforcerProjectile.h"
#include "GCFPS/PickUps/AmmoPickUps/BaseAmmo.h"
#include "GCFPS/PickUps/PickUp.h"
#include "GCFPS/Player/FPCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Math/UnrealMathUtility.h"
#include "Perception/PawnSensingComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Kismet/KismetMathLibrary.h"


AAIEnforcer::AAIEnforcer( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer.SetDefaultSubobjectClass<UProjectileAIShootingComponent>( AAIBase::ShootingComponentName ) )
{		
	// -----
	// Set up capsule component for Enforcer melee attack
	MeleeCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>( TEXT( "MeleeAttackCapsuleComponent" ) );
	MeleeCapsuleComponent->SetupAttachment( GetMesh1P(), "RightFistSocket" );
	MeleeCapsuleComponent->SetCapsuleHalfHeight( 20.0f );
	MeleeCapsuleComponent->SetCapsuleRadius( 35.0f );
	MeleeCapsuleComponent->SetVisibility( true );
	MeleeCapsuleComponent->SetHiddenInGame( false );
	// Setup collision functions
	MeleeCapsuleComponent->OnComponentBeginOverlap.AddDynamic( this, &AAIEnforcer::OnOverlapBegin );
	MeleeCapsuleComponent->OnComponentEndOverlap.AddDynamic( this, &AAIEnforcer::OnOverlapEnd );

	// -----
	// Set up pawn sensing component
	//PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>( TEXT( "PawnSensingComponent" ) );
	//Set the peripheral vision angle
	//PawnSensingComp->SetPeripheralVisionAngle( 30.0f );

	// Set up Enforcer character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator( 0.0f, 300.0f, 0.0f );	// Facing the direction it is walking in (x,y,z)	// turn rate
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;	// m_fSpeed;

	// -----
	// Add actor tag
	Tags.Add( FName( "Turret" ) );

	// Set Enemy Type
	m_eEnemyType = EEnemyType::Enforcer;

	// Set default movement values
	m_fTerminalSpeed = GetCharacterMovement()->MaxWalkSpeed;
	m_fTerminalSpeedDistance = 300.0f;

	// Set default Melee Range
	m_fMeleeRange = 300.0f;


	m_fBallRadius = 60.f;
}

void AAIEnforcer::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	UProjectileAIShootingComponent* pShootingComponent = Cast<UProjectileAIShootingComponent>( GetShootingComponent() );
	if( pShootingComponent )
	{
		m_pACLineTraceShootingComponent = pShootingComponent;
	}
}

// Called when the game starts or when spawned
void AAIEnforcer::BeginPlay()
{
	Super::BeginPlay();

	// FOR TESTING - Change material instance colour
	UMaterialInstanceDynamic* const material_instance = UMaterialInstanceDynamic::Create( GetMesh1P()->GetMaterial( 0 ), this );

	if (material_instance)
	{
		material_instance->SetVectorParameterValue( "BodyColor", FLinearColor( 1.0f, 0.0f, 0.0f, 1.0f ) );	// RED
		GetMesh1P()->SetMaterial( 0, material_instance );
	}
}

// Called every frame
void AAIEnforcer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CalculateRotationForBall(DeltaTime);
}

// Called to bind functionality to input
void AAIEnforcer::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent )
{
	Super::SetupPlayerInputComponent( PlayerInputComponent );
}

// -----

void AAIEnforcer::MeleeTell()
{
	// Particle for tell
	// Will probably change
	//if (MeleeTellParticle)
	//{
	//	UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), MeleeTellParticle, GetActorLocation(), FRotator::ZeroRotator, true );
	//	UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), MeleeTellParticle, GetMesh1P()->GetSocketLocation( FName( "middle_01_r" ) ), FRotator::ZeroRotator, true );
	//}

	Super::MeleeTell();

	//MeleeAttack();
}

void AAIEnforcer::RangedTell()
{
	// Particle for tell
	// Will probably change
	//if (RangedTellParticle)
	//{
	//	UGameplayStatics::SpawnEmitterAtLocation( GetWorld(), RangedTellParticle, GetActorLocation(), FRotator::ZeroRotator, true );
	//}

	Super::RangedTell();

	//RangedAttack();
}

void AAIEnforcer::MeleeAttack()
{
	if (MeleeAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();

		if( AnimInstance != nullptr )
		{
			AnimInstance->Montage_Play( MeleeAnimation, 1.f );
			GEngine->AddOnScreenDebugMessage( -1, 2.0f, FColor::Red, TEXT( "AI Melee Attacking" ) );

			// Update BB Key Values -> Is Currently Attacking = true
			AAIBaseController* pAICon = GetAIController();
			if( pAICon )
			{
				// Get BB
				UBlackboardComponent* pBBComp = pAICon->GetBlackboardComp();
				if( pBBComp )
				{
					// Set currently attacking key to false in BlackBoard
					pBBComp->SetValueAsBool( "IsCurrentlyAttacking", true );
				}
			}
		}
	}
}

// Shoot Attack
void AAIEnforcer::RangedAttack()
{
	//DrawDebugSphere( GetWorld(), GetActorLocation() + 20.0f, 40.0f, 2.0f, FColor::Blue, false, 2.0f, 0, 2 );

	GetShootingComponent()->BeginShooting();
}

void AAIEnforcer::Taunt()
{
	if (TauntAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = GetMesh1P()->GetAnimInstance();

		//PlayAnimMontage( MeleeAnimation );

		if (AnimInstance != nullptr)
		{
			//AnimInstance->Montage_Play( TauntAnimation, 1.0f );

		}
	}

	m_bIsTaunting = false;
	AAIBaseController* pAIController = Cast<AAIBaseController>( GetController() );
	if (pAIController)
	{
		pAIController->GetBlackboardComp()->SetValueAsBool( "IsTaunting", false );
	}
}

void AAIEnforcer::DropPickup()
{
	//EnforcerDrop1RangeMin = Drop1RangeMin;
	//EnforcerDrop1RangeMax = Drop1RangeMax;
	m_fEnforcerDrop1Value = Drop1Value;
	//EnforcerDrop2Value	= Drop2Value;

	Super::DropPickup();
}

void AAIEnforcer::PutToSleep()
{
	// Boilerplate code from AIBase
	Super::PutToSleep();
}

void AAIEnforcer::WakeFromSleep()
{
	// Boilerplate code from AIBase
	Super::WakeFromSleep();

	ToggleMeleeCollision( false );
}

UCurveFloat* AAIEnforcer::GetSpeedControlCurve() const
{
	return m_pSpeedControlCurve;
}

float AAIEnforcer::GetTerminalSpeed() const
{
	return m_fTerminalSpeed;
}

float AAIEnforcer::GetTerminalSpeedDistance() const
{
	return m_fTerminalSpeedDistance;
}

float AAIEnforcer::GetMeleeRange() const
{
	return m_fMeleeRange;
}

bool AAIEnforcer::IsInMeleeRange() const
{
	if( m_pPlayerCharacter )
	{
		// Calculate the Enforcer's Melee Range Squared to compare with its current distance from the player
		const float kfMeleeRangeSquared = m_fMeleeRange * m_fMeleeRange;

		// Get Player Location
		const FVector kv3PlayerLocation = m_pPlayerCharacter->GetActorLocation();

		// Get Delta / Difference Vector
		const FVector kv3Delta = kv3PlayerLocation - GetActorLocation();

		// Calculate Squared Size of Delta Vector
		const float kfDistanceSquared = kv3Delta.SizeSquared();

		// Return whether the Enforcer is in Melee Range
		return kfDistanceSquared <= kfMeleeRangeSquared;
	}

	return false;
}

void AAIEnforcer::CalculateRotationForBall(float fDeltaTime)
{
	const FVector vDeltaVelocity = GetVelocity();
	const float fDeltaVelocityMagnitude = vDeltaVelocity.Size();
	// The Minimum Amount of Velocity needed to do calculate the Ball Rotation
	const float fMinimumDeltaVelocityMagnitude = 0.001f;

	float fRotationAngle = 0.f;

	// No need to do any calculations if the Enforcer is barely moving
	if (fDeltaVelocityMagnitude > fMinimumDeltaVelocityMagnitude)
	{

		const float fRadianToDegreeConversion = 180.f / UKismetMathLibrary::GetPI();
		const float fRadianToDegree = fDeltaVelocityMagnitude * fRadianToDegreeConversion;

		fRotationAngle = fRadianToDegree / m_fBallRadius;

		// The amount to rotate the forward vector by to get it to point downwards
		const FRotator frRotateForwardVectorBy = FRotator(-90.f,0.f, 0.f);
		const FVector vActorNormal = UKismetMathLibrary::Quat_RotateVector(frRotateForwardVectorBy.Quaternion(), GetActorForwardVector());

		float fNormalizeTolerance = 0.0001f;
		// The Axis to rotate the vector around - using AxisAngle
		FVector vRotationAxis = UKismetMathLibrary::Cross_VectorVector(vActorNormal, vDeltaVelocity);
		vRotationAxis.Normalize(fNormalizeTolerance);

		const FRotator frNewBallRotation = UKismetMathLibrary::RotatorFromAxisAndAngle(vRotationAxis, fRotationAngle);

		const float fNewXBallRotation = m_frBallRotation.Roll + frNewBallRotation.Roll;
		const float fNewYBallRotation = m_frBallRotation.Pitch + frNewBallRotation.Pitch;
		const float fNewZBallRotation = m_frBallRotation.Yaw + frNewBallRotation.Yaw;

		m_frBallRotation = FRotator(fNewYBallRotation, fNewZBallRotation, fNewXBallRotation);
	}
}

void AAIEnforcer::ToggleMeleeCollision( bool bCollisionEnabled )
{
	// Toggle collision type
	if( bCollisionEnabled )
	{
		MeleeCapsuleComponent->SetCollisionEnabled( ECollisionEnabled::QueryOnly );
	}
	else
	{
		MeleeCapsuleComponent->SetCollisionEnabled( ECollisionEnabled::NoCollision );
	}

	// Reset hit flag
	m_bHasHitMelee = false;
}

// Melee Damage with melee capsule componenet
// Consider anim notify?
void AAIEnforcer::OnOverlapBegin( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex, bool const FromSweep, FHitResult const& SweepResult )
{
	if (HitActor && (HitActor != this) && OtherComponent)
	{
		AFPCharacter* PlayerChar = Cast<AFPCharacter>( HitActor );

		// If this is the player and this enemy hasn't hit them yet during this melee animation
		if (PlayerChar && !m_bHasHitMelee )
		{
			//UE_LOG(LogTemp, Warning, TEXT("Enforcer melee begin"));

			UDamageType* DamageType;
			UGameplayStatics::ApplyDamage( PlayerChar, MeleeAttackDamage, PlayerChar->GetController(), GetOwner(), DamageType->StaticClass() );
			
			// Set hit flag to true
			m_bHasHitMelee = true;
		}
	}
}

void AAIEnforcer::OnOverlapEnd( UPrimitiveComponent* const OverlappedComponent, AActor* const HitActor, UPrimitiveComponent* OtherComponent, int const BodyIndex )
{
	//UE_LOG( LogTemp, Warning, TEXT( "Enforcer melee end" ) );
}