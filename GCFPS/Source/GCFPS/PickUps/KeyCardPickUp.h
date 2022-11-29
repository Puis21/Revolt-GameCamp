// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KeyCardPickUp.generated.h"

UCLASS()
class GCFPS_API AKeyCardPickUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKeyCardPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
		FVector m_VBoxRadius;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* MyMesh;

	///UPROPERTY(VisibleAnywhere)
		//class UPickUpComponent* PickUpComponent;

	UPROPERTY(VisibleAnywhere)
		bool PickedUp;

	UFUNCTION()
		void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GetPickUpBool() { return PickedUp; }
	void SetPickUpBool(bool b) { PickedUp = b; }
};
