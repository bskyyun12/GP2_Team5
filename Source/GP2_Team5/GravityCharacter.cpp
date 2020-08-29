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

bool AGravityCharacter::GetFlipGravity() const
{
	return bFlipGravity;
}

void AGravityCharacter::SetFlipGravity(bool bNewGravity)
{
	bFlipGravity = bNewGravity;
}

void AGravityCharacter::SetGravityTarget(FVector NewGravityPoint)
{
	GravityPoint = NewGravityPoint;
}

#pragma region Interaction
// Approach Interact
void AGravityCharacter::OnInteract()
{
	if (IsJumping() || bIsGrabbing)
	{
		return;
	}
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

// Click Interact
void AGravityCharacter::OnClick()
{
	if (IsJumping() || bIsGrabbing)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot ClickInteract while jumping nor grabbing. Resetting CurrentClickFocus"));
		ResetClickInteract(CurrentClickFocus);
		return;
	}

	// Define what to detect by left mouse click event
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));	// Interaction
	ObjectTypes.Add(ObjectTypeQuery10);	// Pushbable are always interactables

	FHitResult Hit;
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorForObjects(ObjectTypes, true, Hit);

	if (Hit.bBlockingHit == false)	// Hit Nothing
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Nothing. Reset CurrentClickFocus"));
		ResetClickInteract(CurrentClickFocus);
	}
	else // Hit something
	{
		AActor* HitActor = Hit.GetActor();
		UE_LOG(LogTemp, Warning, TEXT("Clicked: %s"), *HitActor->GetName());

		// Too far to interact
		if (GetDistanceTo(HitActor) > ClickInteractRange)
		{
			//ResetClickInteract(FirstFocus);
			UE_LOG(LogTemp, Warning, TEXT("Too far to interact! Reset FirstFocus, CurrentClickFocus"));
			ResetClickInteract(CurrentClickFocus);

			// Debug ClickInteractRange
			DrawDebugSphere(GetWorld(), GetActorLocation(), ClickInteractRange, 50, FColor::Red, false, 1);
			return;
		}

		// Try getting a component inherited from UClickInteract
		UActorComponent* NewClickFocus = GetComponent<UClickInteract>(HitActor);
		if (NewClickFocus == nullptr) 	// Clicked a Non-ClickInteractable
		{
			ResetClickInteract(CurrentClickFocus);
			return;
		}
		else // Clicked a ClickInteractable
		{
			if (CurrentClickFocus == nullptr)	// CurrentClickFocus is null
			{
				CurrentClickFocus = NewClickFocus;
				IClickInteract::Execute_ClickInteract(CurrentClickFocus);
				UE_LOG(LogTemp, Warning, TEXT("CurrentClickFocus is null. New CurrentClickFocus: %s"), *CurrentClickFocus->GetName());
			}
			else if (CurrentClickFocus == NewClickFocus)	// Clicked the same object
			{
				UE_LOG(LogTemp, Warning, TEXT("Clicked the same object. Reset CurrentClickFocus"));
				ResetClickInteract(CurrentClickFocus);
			}
			else	// Clicked two different objects
			{
				UE_LOG(LogTemp, Warning, TEXT("Clicked two different objects. Reset CurrentClickFocus and NewClickFocus"));

				// Try casting to UGravitySwapComponent
				UGravitySwapComponent* CurrentClickFocusComp = Cast<UGravitySwapComponent>(GetComponent<UGravitySwappable>(CurrentClickFocus->GetOwner()));
				UGravitySwapComponent* NewClickFocusComp = Cast<UGravitySwapComponent>(GetComponent<UGravitySwappable>(NewClickFocus->GetOwner()));

				// if both has UGravitySwapComponent
				if (CurrentClickFocusComp != nullptr && NewClickFocusComp != nullptr)
				{
					// if both has different gravity direction
					if (CurrentClickFocusComp->GetFlipGravity() != NewClickFocusComp->GetFlipGravity())
					{
						// Flip gravity for both
						UE_LOG(LogTemp, Warning, TEXT("Swap gravity"));
						CurrentClickFocusComp->SetFlipGravity(!CurrentClickFocusComp->GetFlipGravity());
						NewClickFocusComp->SetFlipGravity(!NewClickFocusComp->GetFlipGravity());
					}
				}

				ResetClickInteract(CurrentClickFocus);
				ResetClickInteract(NewClickFocus);
			}
		}
	}
}

void AGravityCharacter::ResetClickInteract(UActorComponent*& FocusToReset)
{
	if (FocusToReset == nullptr) { return; }
	if (FocusToReset->Implements<UClickInteract>() == false) { return; }

	IClickInteract::Execute_ResetClickInteract(FocusToReset);
	FocusToReset = nullptr;
}

bool AGravityCharacter::IsJumping()
{
	return CachedGravityMovementyCmp->IsFalling();
}

// End Interact
//////////////////////////////////////////////////////////////////////////
#pragma endregion Interaction