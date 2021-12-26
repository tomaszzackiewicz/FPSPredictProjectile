// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	RootComponent = ProjectileCollision;
	ProjectileCollision->SetSphereRadius(10.0f);
	ProjectileCollision->SetCollisionProfileName(TEXT("BlockAll"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(ProjectileCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bAutoActivate = false;

	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(FName("ExplosionForce"));
	ExplosionForce->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	LaunchBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("LaunchBlast"));
	LaunchBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ImpactBlast = CreateDefaultSubobject<UParticleSystemComponent>(FName("ImpactBlast"));
	ImpactBlast->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ImpactBlast->bAutoActivate = false;

	ExplosionSoundFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplosionSoundFXComponent"));
	ExplosionSoundFXComponent->SetupAttachment(RootComponent);
	ExplosionSoundFXComponent->bAutoActivate = false;

	//InitialLifeSpan = 3.0f;
}

void AGrenade::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//ProjectileMovementComponent->InitialSpeed = InitialSpeed;
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	ProjectileCollision->OnComponentHit.AddDynamic(this, &AGrenade::OnHit);
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrenade::SetVelocity(float LaunchSpeed)
{
	//ProjectileMovementComponent->Velocity = VelocityParam;
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector * LaunchSpeed);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->Activate();
	LaunchBlast->Activate();
}

void AGrenade::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy) {
		FVector ArrowLoc = Hit.ImpactPoint;// -(this->GetActorForwardVector() * 50.0f);
		FName BoneName = Hit.BoneName;
		this->AttachToComponent(Enemy->GetMesh(), FAttachmentTransformRules::KeepWorldTransform, BoneName);
	}
	else {
		ProjectileCollision->SetSimulatePhysics(true);
	}
	
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AGrenade::OnExplode, ExplodeDelay, false);
}

void AGrenade::OnExplode()
{
	LaunchBlast->Deactivate();
	ImpactBlast->Activate();
	if (ExplosionSoundFXComponent && ExplosionSoundFX) {
		ExplosionSoundFXComponent->SetSound(ExplosionSoundFX);
		ExplosionSoundFXComponent->Play();
		ExplosionSoundFXComponent->AttenuationSettings;
	}
	ExplosionForce->FireImpulse();

	//SetRootComponent(ImpactBlast);

	//UGameplayStatics::ApplyRadialDamage(
	//	this,
	//	GrenadeDamage,
	//	GetActorLocation(),
	//	ExplosionForce->Radius, // for consistancy
	//	UDamageType::StaticClass(),
	//	TArray<AActor*>() // damage all actors
	//);

	TArray<AActor*> IgnoreActors;

	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), 200.0f, 100.0f, GetActorLocation(), 600.0f, 800.0f, 2.0f, UDamageType::StaticClass(), IgnoreActors, this, GetInstigatorController(), ECollisionChannel::ECC_Visibility);
	
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AGrenade::OnDisable, 0.5f, false);
	
}

void AGrenade::OnDisable()
{
	this->SetActorHiddenInGame(true);
	this->SetActorEnableCollision(false);
	this->SetActorTickEnabled(false);

	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AGrenade::OnDestroy, 1.5f, false);
	
}

void AGrenade::OnDestroy() {
	this->Destroy();
}