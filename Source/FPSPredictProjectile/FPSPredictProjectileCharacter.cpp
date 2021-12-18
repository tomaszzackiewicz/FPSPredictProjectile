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
#include "UnrealTutorialGrenade.h"
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

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	SplineComp->SetupAttachment(FP_Gun, TEXT("Muzzle"));
	SplineComp->SetHiddenInGame(true, true);

	PathVisualEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PathVisualEffectComp"));
	PathVisualEffectComp->SetupAttachment(SplineComp);

	LaunchVelocity = 1000.0f;
	ProjectileRadius = 10.0f;

	bCalculateProjectilePath = false;
}

void AFPSPredictProjectileCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if (bCalculateProjectilePath) {
	//	FPredictProjectilePathParams Params;
	//	Params.StartLocation = FP_Gun->GetSocketLocation(TEXT("Muzzle"));
	//	Params.LaunchVelocity = FP_Gun->GetSocketRotation(TEXT("Muzzle")).Vector() * LaunchVelocity;
	//	Params.ProjectileRadius = ProjectileRadius;

	//	FPredictProjectilePathResult PathResult;

	//	bool bHit = UGameplayStatics::PredictProjectilePath(GetWorld(), Params, PathResult);

	//	TArray<FVector> PointLocation;

	//	for (auto PathPoint : PathResult.PathData) {
	//		PointLocation.Add(PathPoint.Location);
	//	}

	//	/*if (PointLocation.Num() > 0) {
	//		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Spline points %d"), PointLocation.Num()));
	//	}*/

	//	SplineComp->SetSplinePoints(PointLocation, ESplineCoordinateSpace::World);
	//	
	//}
}

void AFPSPredictProjectileCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATarget::StaticClass(), FoundActors);
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
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSPredictProjectileCharacter::OnFire);
	PlayerInputComponent->BindAction("LaunchProjectile", IE_Pressed, this, &AFPSPredictProjectileCharacter::ShowProjectilePath);
	PlayerInputComponent->BindAction("LaunchProjectile", IE_Released, this, &AFPSPredictProjectileCharacter::LaunchProjectile);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AFPSPredictProjectileCharacter::OnResetVR);

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
	PlayerInputComponent->BindAxis("VelocityModifier", this, &AFPSPredictProjectileCharacter::SetProjectileVelocity);
}

void AFPSPredictProjectileCharacter::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AFPSPredictProjectileProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
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

void AFPSPredictProjectileCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AFPSPredictProjectileCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AFPSPredictProjectileCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AFPSPredictProjectileCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

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

bool AFPSPredictProjectileCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AFPSPredictProjectileCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AFPSPredictProjectileCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AFPSPredictProjectileCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void AFPSPredictProjectileCharacter::ShowProjectilePath()
{
	AFPSPredictProjectileController* FPSPredictProjectileController = Cast<AFPSPredictProjectileController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (!FPSPredictProjectileController) {
		return;
	}

	//bool bHaveAimSolution = false;
	//FVector startLoc = FP_Gun->GetSocketLocation(TEXT("Muzzle"));      //
	//FVector targetLoc = FoundActors[0]->GetActorLocation(); //
	//float arcValue = 0.5f;                       // ArcParam (0.0-1.0)
	//FVector OutLaunchVelocity = FVector::ZeroVector;   // Velocity
	//bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutLaunchVelocity, startLoc, targetLoc, GetWorld()->GetGravityZ(), arcValue);
	//if (bHaveAimSolution)
	//{
	//	FPredictProjectilePathParams predictParams(20.0f, startLoc, OutLaunchVelocity, 15.0f);   // 20: tracing , 15: Max 
	//	predictParams.DrawDebugTime = 15.0f;     //
	//	predictParams.DrawDebugType = EDrawDebugTrace::Type::ForDuration;  // DrawDebugTime  EDrawDebugTrace::Type::ForDuration.
	//	predictParams.OverrideGravityZ = GetWorld()->GetGravityZ();
	//	FPredictProjectilePathResult result;
	//	UGameplayStatics::PredictProjectilePath(this, predictParams, result);
	//}

	FVector OutLaunchVelocity;
	FVector StartLocation = FP_Gun->GetSocketLocation(TEXT("Muzzle"));
	float LaunchSpeed = 1000.0f;
	FVector HitLocation = FPSPredictProjectileController->GetLookVectorHitLocation(); //FoundActors[0]->GetActorLocation();

	// Only for DebugTrace purpose - if not ignore them
	FCollisionResponseParams DummyParams;
	TArray<AActor*> DummyIgnores;

	// Calculate the OutLaunchVelocity
	bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity
	(
		this,
		OutLaunchVelocity,
		StartLocation,
		HitLocation,
		LaunchSpeed,
		false,
		0.f,
		0.f,
		ESuggestProjVelocityTraceOption::DoNotTrace
		, DummyParams, DummyIgnores, true			// comment line to remove Debug DrawLine
	);
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("dddddddddd %f"), LaunchSpeed));
	if (bHaveAimSolution)
	{
		FVector AimDirection = OutLaunchVelocity.GetSafeNormal();
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("kkkkkkkkkkkkkkkkkkkkk %s"), *AimDirection.ToString()));

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AUnrealTutorialGrenade* GrenadeInstance = GetWorld()->SpawnActor<AUnrealTutorialGrenade>(GrenadeClass, FP_Gun->GetSocketLocation(TEXT("Muzzle")), FP_Gun->GetSocketRotation(TEXT("Muzzle")), SpawnParams);

		if (GrenadeInstance) {
			
			//GrenadeInstance->SetVelocity(FVector(UKismetMathLibrary::Abs(OutLaunchVelocity.X), UKismetMathLibrary::Abs(OutLaunchVelocity.Y), UKismetMathLibrary::Abs(OutLaunchVelocity.Z)));
			
			GrenadeInstance->SetVelocity(AimDirection, LaunchSpeed);
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("ggggggggggggggggggggggggggggg %s"), *HitLocation.ToString()));
		return;
		//AimForwardDirection = OutLaunchVelocity.GetSafeNormal();

		//MoveBarrelTowards(AimForwardDirection);
	}

	


	/*if (!SplinePath) {
		return;
	}*/

	/*if (!SplineComp) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ggggggggggggggggggggggggggggg")));
	}
	else {

		SplineComp->SetHiddenInGame(false, true);
		bCalculateProjectilePath = true;
	}*/
}



void AFPSPredictProjectileCharacter::LaunchProjectile()
{
	
	/*if (!SplineComp) {
		return;
	}

	SplineComp->SetHiddenInGame(true, true);
	bCalculateProjectilePath = false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (!GrenadeClass) {
		return;
	}

	AUnrealTutorialGrenade* GrenadeInstance = GetWorld()->SpawnActor<AUnrealTutorialGrenade>(GrenadeClass, FP_Gun->GetSocketLocation(TEXT("Muzzle")), FP_Gun->GetSocketRotation(TEXT("Muzzle")), SpawnParams);*/
	
	
	/*if (GrenadeInstance) {

	}*/

	/*FTransform SpawnTransform;
	SpawnTransform.SetComponents(FQuat::MakeFromEuler(FP_Gun->GetSocketRotation(TEXT("Muzzle")).Euler()), FP_Gun->GetSocketLocation(TEXT("Muzzle")), FVector(1,1,1));

	AUnrealTutorialGrenade* GrenadeInstance2 = GetWorld()->SpawnActorDeferred<AUnrealTutorialGrenade>(GrenadeClass, SpawnTransform, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (GrenadeInstance2) {
		GrenadeInstance2->InitialSpeed = LaunchVelocity;
	}
	UGameplayStatics::FinishSpawningActor(GrenadeInstance2, SpawnTransform);*/

}

void AFPSPredictProjectileCharacter::SetProjectileVelocity(float DeltaParam)
{
	/*LaunchVelocity += DeltaParam * 100.0f;
	LaunchVelocity = FMath::Clamp(LaunchVelocity, 1000.0f, 5000.0f);*/
}
