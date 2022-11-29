// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseAmmo.generated.h"

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	E_AssaultRifle		UMETA(DisplayName = "Assault_Rifle"),
	E_Shotgun			UMETA(DisplayName = "Shotgun"),
	E_GrenadeLauncher	UMETA(DisplayName = "Grenade_Launcher")
};

UCLASS()
class GCFPS_API ABaseAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* MyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radius")
		FVector VBoxRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		int AmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		EAmmoType AmmoType;

	UFUNCTION()
		void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
