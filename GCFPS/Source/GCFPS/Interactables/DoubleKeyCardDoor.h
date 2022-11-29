// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "DoubleKeyCardDoor.generated.h"

UCLASS()
class GCFPS_API ADoubleKeyCardDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoubleKeyCardDoor();
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Card Assign")
		class AKeyCardPickUp* pKeyCard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key Card Assign")
		class AKeyCardPickUp* pKeyCard2;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* MyMesh;

	UPROPERTY(EditAnywhere)
		class USceneComponent* SceneComponent;

	UPROPERTY()
		FVector EndLoc;

	UPROPERTY(EditAnywhere, Category = "Timeline")
		FVector LocOffset;

	UPROPERTY()
		bool CanMove;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void SetBool();

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
		FVector VBoxRadius;

};
