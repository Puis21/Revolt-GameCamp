// Fill out your copyright notice in the Description page of Project Settings.


#include "GCFPS/AI/Seeker/AISeeker.h"

#include "Perception/PawnSensingComponent.h"

#include "GCFPS/AI/Explosion/ExplosionActor.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "../Components/Shooting/LineTraceAIShootingComponent.h"

#include "GCFPS/AI/Seeker/Animation/SeekerAnimInstance.h"

#include "Kismet/GameplayStatics.h"

#include "GCFPS/Player/FPCharacter.h"

#include "DrawDebugHelpers.h"


AAISeeker::AAISeeker(const FObjectInitializer& ObjectInitializer)
	: Super( ObjectInitializer.SetDefaultSubobjectClass<ULineTraceAIShootingComponent>(AAIBase::ShootingComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f);	// Facing the direction it is walking in (x,y,z)	// turn rate
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;	// m_fSpeed;
	//setting rotations

	// Initialize Debug Spheres variables for TargetPoint Trace
	m_fDebugSphereRadius = 10.f;
	m_fDebugSphereSegments = 10.f;

	m_iAmountOfLegs = 4;
	m_fMaxDistanceToTargetPointLocation = 40.f;

	// -----
	// Add actor tag
	Tags.Add( FName( "Seeker" ) );

	// Set Enemy Type
	m_eEnemyType = EEnemyType::Seeker;

	
}


void AAISeeker::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AAISeeker::BeginPlay()
{
	Super::BeginPlay();
	
	
	USeekerAnimInstance* pTempAnimInstance = Cast<USeekerAnimInstance>(GetMesh()->GetAnimInstance());
	if (pTempAnimInstance != nullptr)
	{
		m_pSeekerAnimInstance = pTempAnimInstance;

		InitializeLegs();
	}

	
		
}

void AAISeeker::SeekerBeep()
{
	UGameplayStatics::PlaySoundAtLocation(this,m_SeekerBeep , GetActorLocation(),GetActorRotation(),m_fSeekerBeepVolume);

	GetWorldTimerManager().SetTimer(m_thSeekerBeepTimer, this, &AAISeeker::SeekerBeep, GetSeekerTimer(), false);
}


float AAISeeker::GetDistanceBetweenTwoLocations(FVector& vLocationA, FVector& vLocationB)
{
	float fDistanceBetweenTwoLocations = 0.f;

	// don't do anything if the Vectors are the same
	if (vLocationA != vLocationB)
	{
		const float kfXComp = FMath::Square(vLocationB.X - vLocationA.X);
		const float kfYComp = FMath::Square(vLocationB.Y - vLocationA.Y);
		const float kfZComp = FMath::Square(vLocationB.Z - vLocationA.Z);

		fDistanceBetweenTwoLocations = FMath::Sqrt(kfXComp + kfYComp + kfZComp);
	}

	return fDistanceBetweenTwoLocations;
}

bool AAISeeker::CompareDistanceWithMaxDistance(float fDistanceToCompare)
{
	bool bExceededDistance = false;

	if (fDistanceToCompare > 0)
	{
		bExceededDistance = (fDistanceToCompare >= m_fMaxDistanceToTargetPointLocation) ? true : false;
	}

	return bExceededDistance;
}

void AAISeeker::UpdateTargetPointLocations()
{
	//
	for (int iCurrentLeg = 0; iCurrentLeg < m_iAmountOfLegs; iCurrentLeg++)
	{
		const FVector vLegSocketLocation = GetLegSocketLocation(iCurrentLeg);

		FVector vTargetPointLocation = FVector::ZeroVector;
		bool bLineTraceHit = GetTargetPointLocation(vLegSocketLocation, vTargetPointLocation);
		FString sLineTraceHit = bLineTraceHit ? "True" : "False";
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, sLineTraceHit);
		if (bLineTraceHit)
		{
			// store Trace Hit Location in the array for the respective leg
			m_vLegTargetPointLocation[iCurrentLeg] = vTargetPointLocation;
			//DrawDebugSphere(GetWorld(), m_vLegTargetPointLocation[i], m_fDebugSphereRadius, m_fDebugSphereSegments, FColor::Red, false, 0.f);

			// calculate distance from the current legs position to the Trace Hit Location
			const float fCurrentDistance = GetDistanceBetweenTwoLocations(m_vLegTargetPointLocation[iCurrentLeg], m_vLegLockLocation[iCurrentLeg]);
			// check whether the distance is equal to or is larger than the specified max distance
			const bool bExceedsMaxDistance = CompareDistanceWithMaxDistance(fCurrentDistance);

			// if the distance is equal to or is larger than the specified max distance to TargetPointLocation
			if (bExceedsMaxDistance)
			{
				// set trace hit location to leg lock location - the location to move the leg to
				m_vLegLockLocation[iCurrentLeg] = m_vLegTargetPointLocation[iCurrentLeg];

				//sound effect here for seeker step
				UGameplayStatics::PlaySoundAtLocation(this, WalkingSFX, GetActorLocation(), GetActorRotation(), 0.1);

				// Set respective Leg IK Location
				if (m_pSeekerAnimInstance != nullptr)
				{
					m_pSeekerAnimInstance->SetLegLocation(iCurrentLeg, m_vLegLockLocation[iCurrentLeg]);
				}

			}
		}
	}
}

void AAISeeker::InitializeLegs()
{
	for (int iCurrentLeg = 0; iCurrentLeg < m_iAmountOfLegs; iCurrentLeg++)
	{
		const FVector vLegSocketLocation = GetLegSocketLocation(iCurrentLeg);

		const FVector vNewLegWorldLocation = GetTargetPointLocation(vLegSocketLocation);

		m_vLegLockLocation[iCurrentLeg] = vNewLegWorldLocation;

		if (m_pSeekerAnimInstance != nullptr)
		{
			m_pSeekerAnimInstance->SetLegLocation(iCurrentLeg, m_vLegLockLocation[iCurrentLeg]);
		}
	}
}

FVector AAISeeker::GetTargetPointLocation(FVector vSocketLocation)
{
	// not used right now

	FVector vNewLegLocation = FVector::ZeroVector;

	// World Location of the respective Leg Socket
	const FVector vLegLocation = vSocketLocation;
	// Start Location of the Trace for the respective Leg
	const float fMaxHeightTrace = 30.f;
	const float fMinHeightTrace = 70.f;
	const FVector vLegTraceStart = vLegLocation + FVector(0.f, 0.f, fMaxHeightTrace);
	// End Location of the Trace for the respective Leg
	const FVector vLegTraceEnd = vLegLocation - FVector(0.f, 0.f, fMinHeightTrace);

	FHitResult sHitResult;
	FCollisionQueryParams sCollisionParams;
	sCollisionParams.AddIgnoredActor(this);

	//DrawDebugLine(GetWorld(), vLegTraceStart, vLegTraceEnd, FColor::Red, false, 0.f);

	if (GetWorld() != nullptr)
	{
		bool bHit = false;
		bHit = GetWorld()->LineTraceSingleByChannel(sHitResult, vLegTraceStart, vLegTraceEnd, ECC_Visibility, sCollisionParams);
		if (bHit)
		{
			vNewLegLocation = sHitResult.Location;
		}
	}

	return vNewLegLocation;
}

bool AAISeeker::GetTargetPointLocation(FVector vLegSocketLocation, FVector& vTargetPointLocation)
{
	bool bLineTraceHit = false;

	// Start Location of the Trace for the respective Leg
	// the following two variables need better names
	const float fTraceApexHeightFromSocketLocation = 30.f;
	const float fTraceNadirHeightFromSocketLocation = 70.f;
	const FVector vLegTraceStart = vLegSocketLocation + FVector(0.f, 0.f, fTraceApexHeightFromSocketLocation);
	// End Location of the Trace for the respective Leg
	const FVector vLegTraceEnd = vLegSocketLocation - FVector(0.f, 0.f, fTraceNadirHeightFromSocketLocation);

	FHitResult sHitResult;
	FCollisionQueryParams sCollisionParams;
	sCollisionParams.AddIgnoredActor(this);

	if (GetWorld() != nullptr)
	{
		bLineTraceHit = GetWorld()->LineTraceSingleByChannel(sHitResult, vLegTraceStart, vLegTraceEnd, ECC_Visibility, sCollisionParams);
		if (bLineTraceHit)
		{
			vTargetPointLocation = sHitResult.Location;
		}
	}

	return bLineTraceHit;
}

FVector AAISeeker::GetLegSocketLocation(int iCurrentLeg)
{
	// Vector to return
	FVector vLegSocketLocation = FVector::ZeroVector;

	switch (iCurrentLeg)
	{
	case 0:
		vLegSocketLocation = GetMesh()->GetSocketLocation("FrontLeftLegSocket");
		break;
	case 1:
		vLegSocketLocation = GetMesh()->GetSocketLocation("FrontRightLegSocket");
		break;
	case 2:
		vLegSocketLocation = GetMesh()->GetSocketLocation("BackLeftLegSocket");
		break;
	case 3:
		vLegSocketLocation = GetMesh()->GetSocketLocation("BackRightLegSocket");
		break;
	}

	return vLegSocketLocation;
}


void AAISeeker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	SeekerPlayerDistanceCheck();
	

	UpdateTargetPointLocations();

}

void AAISeeker::SeekerExplode()
{

	FActorSpawnParameters ActorSpawnParameters;

	FTransform ActorTransform = GetTransform();

	GetWorld()->SpawnActor<AExplosionActor>(SeekerExplosionActorClass, ActorTransform, ActorSpawnParameters);
	//Spawns the EXplosion actor when the Seeker explodes

	if (m_cSeekerExplosionCameraShakeClass != nullptr)
	{
		const FVector vCameraShakeEpicenter = GetActorLocation();
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), m_cSeekerExplosionCameraShakeClass, vCameraShakeEpicenter, m_fCameraShakeInnerRadius, m_fCameraShakeOuterRadius, m_fCameraShakeFallOff, false);
	}

	GetWorldTimerManager().ClearTimer(m_thSeekerBeepTimer);
	//Reset timer when seeker dies
	OnDeath();
	
}

void AAISeeker::PutToSleep()
{
	// Boilerplate code from AIBase
	Super::PutToSleep();

	GetWorldTimerManager().ClearTimer(m_thSeekerBeepTimer);

	SetActorTickEnabled(false);

	
	
}

void AAISeeker::SeekerPlayerDistanceCheck()
{
	
	AFPCharacter* Character = Cast<AFPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (Character != nullptr)
	{

	FVector Playerloc = Character->GetActorLocation();// Player location

	FVector SeekerLoc = GetActorLocation();//Seeker Location

	float fDistanceBetween = (SeekerLoc - Playerloc).Size();

	//When the player gets into a certain range the beep frequency changes, meaning that it will become faster when the seeker is closer to the player
	if(fDistanceBetween <= 1000.0f)
	{
		//m_fSeekerBeepTimer = 0.2f;
		SetSeekerTimer(0.2);
	}
	else if (fDistanceBetween <= 1400.0f && fDistanceBetween > 1200.0f )
	{
		//m_fSeekerBeepTimer = 0.4f;
		SetSeekerTimer(0.4);
	}
	else if (fDistanceBetween <= 1600.0f && fDistanceBetween > 1400.0f)
	{
		
		//m_fSeekerBeepTimer = 0.6f;
		SetSeekerTimer(0.6);
	}
	else if (fDistanceBetween >= 1600.0f )
	{
		//m_fSeekerBeepTimer = 1.0f;
		SetSeekerTimer(1);
	}

	}

	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(TEXT("TimerSpeed Seeker: %f"), m_fSeekerBeepTimer));
	
}

void AAISeeker::WakeFromSleep()
{
	// Boilerplate code from AIBase
	Super::WakeFromSleep();

	GetWorldTimerManager().SetTimer(m_thSeekerBeepTimer, this, &AAISeeker::SeekerBeep, GetSeekerTimer(), false);

	SetActorTickEnabled(true);

	
	
}
;

void AAISeeker::DropPickup()
{
	m_fSeekerDrop1Value = Drop1Value;

	Super::DropPickup();
}
