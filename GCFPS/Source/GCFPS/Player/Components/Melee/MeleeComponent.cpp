// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/Player/Components/Melee/MeleeComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "GCFPS/AI/Base/AIBase.h"

// Sets default values for this component's properties
UMeleeComponent::UMeleeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	m_fMeleeDamage = 20.f;
	// was 140 before + / 1.4
	m_fMeleeRange = 100.f;
	m_bIsMeleeActive = false;
}


// Called when the game starts
void UMeleeComponent::BeginPlay()
{
	Super::BeginPlay();

	// Get Owning Character and do nullptr check
	ACharacter* tempOwningCharacter = Cast<ACharacter>(GetOwner());
	if (nullptr != tempOwningCharacter)
	{
		m_pOwningCharacter = tempOwningCharacter;
	}

	//
	if (m_pMeleeDashCurve)
	{
		FOnTimelineFloat MeleeDashTimelineProgress;
		MeleeDashTimelineProgress.BindUFunction(this, FName("MeleeDashProgress"));
		m_TLMeleeDashTimeline.AddInterpFloat(m_pMeleeDashCurve, MeleeDashTimelineProgress);
	}
}

bool UMeleeComponent::SweepForTarget(bool bDrawDebug)
{
	bool bTargetFound = false;

	// The current Location of our Player Character
	FVector ActorLocation = FVector::ZeroVector;
	// The current Forward Vector of our Player Character
	FVector ActorForwardVector = FVector::ZeroVector;
	FQuat ActorRotation = FQuat(0.f, 0.f, 0.f, 0.f);
	FVector BoxTraceLocation = FVector::ZeroVector;

	// Collision Query Parameters are needed to ignore our OwningCharacter when sweeping the world for targets
	FCollisionQueryParams CollisionParams;

	if (nullptr != m_pOwningCharacter)
	{
		ActorLocation = m_pOwningCharacter->GetActorLocation();
		ActorLocation.Z += 50.f;
		ActorForwardVector = m_pOwningCharacter->GetActorForwardVector();
		ActorRotation = m_pOwningCharacter->GetControlRotation().Quaternion();
		// was m_fMeleeRange / 1.4f before
		BoxTraceLocation = ActorLocation + (ActorForwardVector * (m_fMeleeRange));

		CollisionParams.AddIgnoredActor(m_pOwningCharacter);
	}

	TArray<FHitResult> Hits;

	// Y and Z Scale of the Box Trace Shape
	// Allows us to scale the Y and Z Components of the Trace uniformly
	float fBoxTraceYZ = 30.f;
	FCollisionShape BoxTraceShape = FCollisionShape::MakeBox(FVector(m_fMeleeRange, fBoxTraceYZ, fBoxTraceYZ));

	// The DebugColor will change depending on whether a target was found with a sweep or not
	// Red = no target found || Green = a target was found
	FColor DebugShapeColor = FColor::Red;

	bool bSweepHit = GetWorld()->SweepMultiByProfile(Hits, BoxTraceLocation, BoxTraceLocation, ActorRotation, TEXT("Melee"), BoxTraceShape, CollisionParams);
	if (bSweepHit)
	{
		// start off with a large closest distance, as it will keep getting replaced with a smaller value,
		// if the distance to the next possible target is smaller than the distance to current target
		float ClosestDistanceToPlayer = m_fLargestDistance;
		AActor* ClosestTarget = nullptr;

		// check for closest enemy as many times as there are HitResult Array Elements
		for (int i = 0; i < Hits.Num(); i++)
		{
			// Get the next Actor to check whether it's the closest target
			AActor* PossibleTarget = Hits[i].GetActor();
			if (nullptr != PossibleTarget)
			{
				// To be used to calculate the distance of the next possible target to our player character
				// and determine whether this possible target is closer to our player character than the previous
				// possible target
				FVector PossibleTargetLocation = PossibleTarget->GetActorLocation();

				float DistanceToPossibleTarget = FVector(ActorLocation - PossibleTargetLocation).Size();
				// if distance to current possible target is shorter than the distance to the previous closest distance
				// set the current possible target to be the closest target
				if (DistanceToPossibleTarget < ClosestDistanceToPlayer)
				{
					DebugShapeColor = FColor::Green;
					ClosestDistanceToPlayer = DistanceToPossibleTarget;
					ClosestTarget = Hits[i].GetActor();
				}
			}
		}
		m_pTargetActorToDamage = ClosestTarget;
		bTargetFound = true;
	}
	else
	{
		// if no target was found set debug color to red 
		DebugShapeColor = FColor::Red;
	}

	if (bDrawDebug)
	{
		DrawDebugBox(GetWorld(), BoxTraceLocation, FVector(m_fMeleeRange, 30.f, 30.f), ActorRotation, DebugShapeColor, true, 10.f);
	}

	return bTargetFound;
}

void UMeleeComponent::MeleeDashProgress(float fMeleeDashDistanceProgress)
{
	if (nullptr != m_pTargetActorToDamage)
	{
		// Lerp our Player Character's Location towards the target actor's location to simulate a dash/lunge based on the provided float curve
		FVector TargetLocation = m_pTargetActorToDamage->GetActorLocation();
		FVector NewActorLocation = UKismetMathLibrary::VLerp(m_tInitialPlayerTransform.GetLocation(), TargetLocation, fMeleeDashDistanceProgress);
		m_pOwningCharacter->SetActorLocation(NewActorLocation, true);
	}

	// reset values if the timeline is at the end
	if (m_TLMeleeDashTimeline.GetPlaybackPosition() >= m_TLMeleeDashTimeline.GetTimelineLength())
	{
		m_pTargetActorToDamage = nullptr;
		m_bIsMeleeActive = false;
	}
}

void UMeleeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (m_TLMeleeDashTimeline.IsPlaying())
	{
		m_TLMeleeDashTimeline.TickTimeline(DeltaTime);
	}
}

void UMeleeComponent::DealDamage()
{
	if (m_pTargetActorToDamage != nullptr && m_pOwningCharacter != nullptr)
	{
		UDamageType* DamageType;
		UGameplayStatics::ApplyDamage(m_pTargetActorToDamage, m_fMeleeDamage, m_pOwningCharacter->GetInstigatorController(), GetOwner(), DamageType->StaticClass());
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::SanitizeFloat(m_fMeleeDamage));

		AAIBase* pAIBase = Cast<AAIBase>(m_pTargetActorToDamage);
		if (pAIBase != nullptr)
		{
			if (pAIBase->GetEnemyType() != EEnemyType::Turret)
			{
				const FVector kvActorToDamageLocation = m_pTargetActorToDamage->GetActorLocation();
				const FVector kvOwningCharacterLocation = m_pOwningCharacter->GetActorLocation();

				FVector vDirectionVector = kvActorToDamageLocation - kvOwningCharacterLocation;
				// we don't want the enemy to be pushed into the ground or the air
				vDirectionVector.Z = 0.f;
				const float kfNormalizeTolerance = 0.001f;
				vDirectionVector.Normalize(kfNormalizeTolerance);
				vDirectionVector *= m_fMeleeLaunchMultiplier;

				UGameplayStatics::PlaySoundAtLocation(this, EnemyHitSFX, m_pOwningCharacter->GetActorLocation());

				pAIBase->LaunchCharacter(vDirectionVector, true, false);
			}
		}
		m_pTargetActorToDamage = nullptr;
	}
}

void UMeleeComponent::MeleeAttack()
{
	// whether we're using the melee attack already - only enable usage of melee attack, if not already in use
	if (!m_bIsMeleeActive)
	{
		// find target in the world in front of the player character to use the melee attack on
		bool bTargetFound = SweepForTarget(false);
		if (bTargetFound && nullptr != m_pOwningCharacter)
		{
			// if target was found, store the transform of the player character as the initial transform,
			// because the transform from the frame of when the melee attack was triggered is needed later on
			
			m_tInitialPlayerTransform = m_pOwningCharacter->GetActorTransform();
				
			// activate Timeline - always play from start, just in case it get's stopped halfway by some other code
			m_TLMeleeDashTimeline.PlayFromStart();
			m_bIsMeleeActive = true;

		}
	}
}