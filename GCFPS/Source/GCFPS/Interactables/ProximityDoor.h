// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "ProximityDoor.generated.h"

class UCurveVector;

UCLASS()
class GCFPS_API AProximityDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProximityDoor();

	UFUNCTION()
		void TimelineProgress(float value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimeline CurveTimeline;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		UCurveVector* CurveVector;

	UPROPERTY()
		FVector StartLoc;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* MyMesh;

	UPROPERTY(EditAnywhere)
		class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
		FVector VBoxRadius;

	UPROPERTY()
		FVector EndLoc;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		FVector LocOffset;

	UPROPERTY()
		bool CanMove;

	UPROPERTY()
		bool CanBack;

	UFUNCTION()
		void SetBool();

	UFUNCTION()
		void ReverseAnim();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
