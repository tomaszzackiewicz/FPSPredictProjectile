// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnrealTutorialGrenade.generated.h"

class USphereComponent;
class StaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class FPSPREDICTPROJECTILE_API AUnrealTutorialGrenade : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	USphereComponent* ProjectileCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;
	
public:	
	// Sets default values for this actor's properties
	AUnrealTutorialGrenade();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	float InitialSpeed;

	UPROPERTY()
	FVector InitialVelocity;

	void SetVelocity(FVector VelocityParam, float LaunchSpeed);
};
