// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/GameplayStatics.h"
#include "EnemySpawner.h"
#include "Components/CapsuleComponent.h"
#include "EnemyAIController.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CurrentHealth = 100.0f;
	bIsRespawned = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {

	float DamageToApply = FMath::Clamp(Damage, 0.0f, CurrentHealth);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Hit!!!!!!!!!!!!!!!")));

	CurrentHealth -= DamageToApply;
	if (CurrentHealth <= 0)
	{
		SimulatePhyscis();
	}
	
	return DamageToApply;
}

void AEnemy::SimulatePhyscis(){
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->DestroyComponent();

	AEnemyAIController* EnemyAIController = Cast<AEnemyAIController>(this->GetController());
	if (EnemyAIController)
	{
		EnemyAIController->UnPossess();
	}

	if (!bIsRespawned) {
		bIsRespawned = true;

		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &AEnemy::OnRespawn, 2.0f, false);
	}
}

void AEnemy::OnRespawn()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0) {
		AEnemySpawner* EnemySpawner = Cast<AEnemySpawner>(FoundActors[0]);

		if (EnemySpawner) {
			EnemySpawner->SpawnEnemy();
			FTimerHandle Timer;
			GetWorld()->GetTimerManager().SetTimer(Timer, this, &AEnemy::OnDestroy, 1.0f, false);
		}
			
	}
}

void AEnemy::OnDestroy()
{
	this->Destroy();
}

