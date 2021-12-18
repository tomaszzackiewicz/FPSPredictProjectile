// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealTutorialGrenade.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AUnrealTutorialGrenade::AUnrealTutorialGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollision = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollision"));
	RootComponent = ProjectileCollision;
	ProjectileCollision->SetSphereRadius(10.0f);
	ProjectileCollision->SetCollisionProfileName(TEXT("BlockAll"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(ProjectileCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bAutoActivate = false;
	/*ProjectileMovementComponent->InitialSpeed = 10000.0f;
	ProjectileMovementComponent->Velocity = FVector(1, 0, 0);*/

	InitialLifeSpan = 3.0f;

	//InitialSpeed = 1000.0f;

}

void AUnrealTutorialGrenade::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	//ProjectileMovementComponent->InitialSpeed = InitialSpeed;
}

// Called when the game starts or when spawned
void AUnrealTutorialGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnrealTutorialGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUnrealTutorialGrenade::SetVelocity(FVector VelocityParam, float LaunchSpeed)
{
	//ProjectileMovementComponent->Velocity = VelocityParam;
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector * LaunchSpeed);
	ProjectileMovementComponent->Activate();
}

