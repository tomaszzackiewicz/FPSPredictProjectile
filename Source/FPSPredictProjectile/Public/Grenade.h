// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

class USphereComponent;
class StaticMeshComponent;
class UProjectileMovementComponent;
class URadialForceComponent;
class UParticleSystemComponent;
class UAudioComponent;
class USoundBase;

UCLASS()
class FPSPREDICTPROJECTILE_API AGrenade : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	USphereComponent* ProjectileCollision = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	URadialForceComponent* ExplosionForce = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* LaunchBlast = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ImpactBlast = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* ExplosionSoundFXComponent = nullptr;
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

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

	void SetVelocity(float LaunchSpeed);

private:

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	USoundBase* ExplosionSoundFX;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	float GrenadeDamage = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Grenade", meta = (AllowPrivateAccess = "true"))
	float ExplodeDelay = 5.0f;

	UFUNCTION()
	void OnExplode();

	UFUNCTION()
	void OnDisable();

	UFUNCTION()
	void OnDestroy();
};
