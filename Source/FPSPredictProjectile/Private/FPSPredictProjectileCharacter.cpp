// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSPredictProjectileCharacter.h"
#include "FPSPredictProjectileProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "Grenade.h"
#include "Target.h"
#include "Kismet/KismetMathLibrary.h"
#include "FPSPredictProjectileController.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPSPredictProjectileCharacter

AFPSPredictProjectileCharacter::AFPSPredictProjectileCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	LaunchSpeed = 2000.0f;
	ProjectileRadius = 10.0f;

	bCalculateProjectilePath = false;
}

void AFPSPredictProjectileCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AFPSPredictProjectileCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFPSPredictProjectileCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSPredictProjectileCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSPredictProjectileCharacter::LaunchProjectile);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSPredictProjectileCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSPredictProjectileCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSPredictProjectileCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSPredictProjectileCharacter::LookUpAtRate);
}

void AFPSPredictProjectileCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AFPSPredictProjectileProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
	}

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void AFPSPredictProjectileCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPSPredictProjectileCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSPredictProjectileCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSPredictProjectileCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AFPSPredictProjectileCharacter::LaunchProjectile()
{
	AFPSPredictProjectileController* FPSPredictProjectileController = Cast<AFPSPredictProjectileController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (!FPSPredictProjectileController) {
		return;
	}

	//bool bHaveAimSolution = false;
	//FVector OutLaunchVelocity = FVector::ZeroVector;   // Velocity
	//FVector StartLocation = FP_Gun->GetSocketLocation(TEXT("Muzzle"));
	//FVector HitLocation = FPSPredictProjectileController->GetLookVectorHitLocation();
	//float ArcValue = 0.5f;                       // ArcParam (0.0-1.0)
	//
	//bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutLaunchVelocity, StartLocation, HitLocation, GetWorld()->GetGravityZ(), ArcValue);
	//if (bHaveAimSolution)
	//{
	//	FPredictProjectilePathParams predictParams(20.0f, StartLocation, OutLaunchVelocity, 15.0f);   // 20: tracing , 15: Max 
	//	predictParams.DrawDebugTime = 15.0f;     //
	//	predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;  // DrawDebugTime  EDrawDebugTrace::Type::ForDuration.
	//	predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
	//	FPredictProjectilePathResult result;
	//	UGameplayStatics::PredictProjectilePath(this, predictParams, result);
	//}

	FVector OutLaunchVelocity = FVector::ZeroVector;
	FVector StartLocation = FP_Gun->GetSocketLocation(TEXT("Muzzle"));
	FVector HitLocation = FPSPredictProjectileController->GetLookVectorHitLocation();
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("HitLocation %s"), *HitLocation.ToString()));
	FCollisionResponseParams DummyParams;
	TArray<AActor*> DummyIgnores;

	 //Calculate the OutLaunchVelocity
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity
	(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		10.f,
		100.f,
		ESuggestProjVelocityTraceOption::DoNotTrace
		//, DummyParams, DummyIgnores, true			// comment line to remove Debug DrawLine
	);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("LaunchSpeed %f"), LaunchSpeed));
	
	if (bHaveAimSolution){
		
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AGrenade* GrenadeInstance = GetWorld()->SpawnActor<AGrenade>(GrenadeClass, FP_Gun->GetSocketLocation(TEXT("Muzzle")), FP_Gun->GetSocketRotation(TEXT("Muzzle")), SpawnParams);

		if (GrenadeInstance) {
			GrenadeInstance->SetVelocity(LaunchSpeed);
		}
	}
	
}
