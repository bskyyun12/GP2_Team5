// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityCharacter.h"
#include "GravityMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractionInterface.h"
#include "GravityCube.h"
#include "GravitySwappable.h"
#include "ClickInteract.h"	
#include "DrawDebugHelpers.h"
#include "GravitySwapComponent.h"

// Sets default values
AGravityCharacter::AGravityCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	CachedGravityMovementyCmp = Cast<UGravityMovementComponent>(GetMovementComponent());

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Rotation of the character should not affect rotation of boom
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 75.f);
	CameraBoom->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	// Create a camera and attach to boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	SideViewCameraComponent->bUsePawnControlRotation = false; // We don't want the controller rotating the camera

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Face in the direction we are moving..
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->GravityScale = 2.f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MaxFlySpeed = 600.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	// Interaction
	InteractBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction box"));
	InteractBox->SetupAttachment(RootComponent);
}

bool AGravityCharacter::GetFlipGravity() const
{
	return bFlipGravity;
}

void AGravityCharacter::SetFlipGravity(bool bNewGravity)
{
	bFlipGravity = bNewGravity;
	//OnGravityChanged.Broadcast(bNewGravity);
}

void AGravityCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGravityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep Character locked in X axis at all times
	FVector ConstrainedLocation = GetActorLocation();
	ConstrainedLocation.X = 0.0f;
	SetActorLocation(ConstrainedLocation);

	// Apply gravity to character
	const FVector OldGravityDir = CachedGravityMovementyCmp->GetGravityDirection();
	FVector NewGravityDir = GravityPoint - GetActorLocation();
	NewGravityDir.Normalize();
	if (bFlipGravity)
	{
		NewGravityDir = NewGravityDir * -1.f;
	}

	//NewGravityDir = FMath::VInterpTo(OldGravityDir, NewGravityDir, DeltaTime, GravityChangeSpeed);
	CachedGravityMovementyCmp->SetGravityDirection(NewGravityDir);


	// Calculate three vector to make the rotation space for camera
	const FVector ForwardVector{ -1.0f, 0.0f, 0.0f };
	const FVector UpVector = UKismetMathLibrary::GetDirectionUnitVector(GravityPoint, GetActorLocation());
	const FVector RightVector = FVector::CrossProduct(UpVector, ForwardVector);

	// Calculate the rotation and set rotation
	FRotator TargetRotation = UKismetMathLibrary::MakeRotationFromAxes(ForwardVector, RightVector, UpVector);
	TargetRotation = FMath::RInterpTo(CameraBoom->GetComponentRotation(), TargetRotation, DeltaTime, 15);
	CameraBoom->SetWorldRotation(TargetRotation);
}

void AGravityCharacter::MoveRight(float Val)
{
	// Add movement with consideration to the direction of camera
	FVector LocalMove{ 0.0f, 1.0f, 0.0f };
	FQuat CameraRotation = CameraBoom->GetComponentRotation().Quaternion();
	FVector TranslatedMove = CameraRotation * LocalMove;
	TranslatedMove.X = 0.0f;
	AddMovementInput(TranslatedMove, Val);
}

void AGravityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGravityCharacter::MoveRight);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AGravityCharacter::OnInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AGravityCharacter::OnInteractReleased);
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Released, this, &AGravityCharacter::OnClick);
}

void AGravityCharacter::SetGravityTarget(FVector NewGravityPoint)
{
	GravityPoint = NewGravityPoint;
}

#pragma region Interaction
// Interact
void AGravityCharacter::OnInteract()
{
	ApproachInteractableComp = TryGetApproachInteractableComp();
	if (ApproachInteractableComp != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interacts with : %s"), *ApproachInteractableComp->GetName());
		IApproachInteract::Execute_Interact(ApproachInteractableComp);
	}
}

void AGravityCharacter::OnInteractReleased()
{
	if (ApproachInteractableComp != nullptr)
	{
		IApproachInteract::Execute_InteractReleased(ApproachInteractableComp);
	}
	ApproachInteractableComp = nullptr;
}

UActorComponent* AGravityCharacter::TryGetApproachInteractableComp()
{
	TArray<AActor*> OverlappingActors;
	InteractBox->GetOverlappingActors(OverlappingActors);
	if (OverlappingActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OverlappingActors : 0"));
		return nullptr;
	}

	//if (ApproachInteractable != nullptr)
	//{
	//	ApproachInteractable->HideInteractionWidget();
	//	ApproachInteractable = nullptr;
	//}

	// Get the closest actor among the Overlapping actors
	AActor* ClosestActor = OverlappingActors[0];
	for (auto CurrentActor : OverlappingActors)
	{
		if (GetDistanceTo(CurrentActor) < GetDistanceTo(ClosestActor))
		{
			ClosestActor = CurrentActor;
		}
	}

	return GetComponent<UApproachInteract>(ClosestActor);
}

void AGravityCharacter::OnClick()
{

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));	// Interaction

	FHitResult Hit;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorForObjects(ObjectTypes, true, Hit);

	if (Hit.bBlockingHit)
	{
		AActor* HitActor = Hit.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("Clicked: %s"), *HitActor->GetName());

		if (GetDistanceTo(HitActor) > ClickInteractRange)
		{
			// Too far to interact
			UE_LOG(LogTemp, Warning, TEXT("Too far to interact! Reset FirstFocus"));
			DrawDebugSphere(GetWorld(), GetActorLocation(), ClickInteractRange, 50, FColor::Red, false, 1);

			// Reset GravitySwap
			FirstFocus = nullptr;

			// Reset ClickInteract
			if (CurrentClickFocus != nullptr)
			{
				IClickInteract::Execute_ResetClickInteract(CurrentClickFocus);
				CurrentClickFocus = nullptr;
			}
			return;
		}

		// GravitySwap
		UActorComponent* GravitySwappable = GetComponent<UGravitySwappable>(HitActor);

		//TScriptInterface<IGravitySwappable> GravitySwappable = TScriptInterface<IGravitySwappable>(HitActor);
		if (GravitySwappable != nullptr)
		{
			if (FirstFocus == nullptr)
			{
				// Set FirstFocus
				UE_LOG(LogTemp, Warning, TEXT("FirstFocus is null. Setting FirstFocus..."));
				FirstFocus = GravitySwappable;
			}
			else if (FirstFocus == GravitySwappable)
			{
				// Selected the same object.
				UE_LOG(LogTemp, Warning, TEXT("Same object is clicked. Reset FirstFocus"));
				FirstFocus = nullptr;
			}
			else if (FirstFocus != GravitySwappable)
			{
				// Selected two different objects.
				UE_LOG(LogTemp, Warning, TEXT("Different object is clicked. Swap gravity & Reset FirstFocus"));

				UGravitySwapComponent* FirstFocusComp = Cast<UGravitySwapComponent>(FirstFocus);
				UGravitySwapComponent* GravitySwappableComp = Cast<UGravitySwapComponent>(GravitySwappable);

				if (FirstFocusComp->GetFlipGravity() != GravitySwappableComp->GetFlipGravity())
				{
					FirstFocusComp->SetFlipGravity(!FirstFocusComp->GetFlipGravity());
					GravitySwappableComp->SetFlipGravity(!GravitySwappableComp->GetFlipGravity());
				}

				FirstFocus = nullptr;
			}
		}
		else // IGravitySwappable is nullptr
		{
			FirstFocus = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("Not IGravitySwappable. Reset FirstFocus"));
		}

		// ClickInteract
		UActorComponent* ClickInteractable = GetComponent<UClickInteract>(HitActor);
		if (ClickInteractable != nullptr)
		{
			// When player clicks the second clickable object
			if (CurrentClickFocus != nullptr)
			{
				IClickInteract::Execute_ResetClickInteract(CurrentClickFocus);
				IClickInteract::Execute_ResetClickInteract(ClickInteractable);
				CurrentClickFocus = nullptr;
				return;
			}

			CurrentClickFocus = ClickInteractable;
			IClickInteract::Execute_ClickInteract(CurrentClickFocus);
		}
		else
		{
			if (CurrentClickFocus)
			{
				IClickInteract::Execute_ResetClickInteract(CurrentClickFocus);
			}
			CurrentClickFocus = nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Nothing. Reset FirstFocus"));
		// Reset GravitySwap
		FirstFocus = nullptr;

		// Reset ClickInteract
		if (CurrentClickFocus != nullptr)
		{
			IClickInteract::Execute_ResetClickInteract(CurrentClickFocus);
			CurrentClickFocus = nullptr;
		}
	}
}
// End Interact
//////////////////////////////////////////////////////////////////////////
#pragma endregion Interaction