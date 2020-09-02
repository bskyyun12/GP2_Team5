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
	if (Val == 0) { return; }

	// Add movement with consideration to the direction of camera
	FVector LocalMove{ 0.0f, 1.0f, 0.0f };
	FQuat CameraRotation = CameraBoom->GetComponentRotation().Quaternion();
	FVector TranslatedMove = CameraRotation * LocalMove;
	TranslatedMove.X = 0.0f;
	AddMovementInput(TranslatedMove, Val);

	// Reset CurrentClickFocus
	ResetClickInteract(CurrentClickFocus);
}

void AGravityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AGravityCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGravityCharacter::MoveRight);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AGravityCharacter::OnApproachInteract);
	//PlayerInputComponent->BindAction("Interact", IE_Released, this, &AGravityCharacter::OnInteractReleased);
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Released, this, &AGravityCharacter::OnClickInteract);
}

bool AGravityCharacter::GetFlipGravity() const
{
	return bFlipGravity;
}

void AGravityCharacter::SetFlipGravity(bool bNewGravity)
{
	bFlipGravity = bNewGravity;
}

void AGravityCharacter::AddCollectible(ACollectible* Collectible)
{
	if (Collectible != nullptr)
	{
		if (Collectible->GetCount() <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Collitible has invalid count"));
		}

		if (Collectibles.Contains(Collectible->GetType()))
		{
			Collectibles[Collectible->GetType()] += Collectible->GetCount();
		}
		else
		{
			Collectibles.Add(Collectible->GetType() , Collectible->GetCount());
		}

		// Notify blueprint 
		OnCollectibleAdded(Collectible->GetType(), Collectibles[Collectible->GetType()]);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Collitible was nullptrs"));
	}
}

void AGravityCharacter::SetGravityTarget(FVector NewGravityPoint)
{
	GravityPoint = NewGravityPoint;
}

#pragma region Jump

void AGravityCharacter::Jump()
{
	if (bIsGrabbing) { return; }
	ResetClickInteract(CurrentClickFocus);

	ACharacter::Jump();
}

bool AGravityCharacter::IsJumping()
{
	return CachedGravityMovementyCmp->IsFalling();
}

#pragma endregion

#pragma region Interaction
// Approach Interact
void AGravityCharacter::OnApproachInteract()
{
	// return if the player is currently jumping or grabbing something
	if (IsJumping() || bIsGrabbing)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot ApproachInteract while jumping nor grabbing. Calling OnInteractReleased"));
		OnApproachInteractReleased();
		return;
	}

	// Reset CurrentClickFocus
	ResetClickInteract(CurrentClickFocus);

	// Try to get a ApproachInteractableComponent and execute its Interact() method
	ApproachInteractableComp = TryGetApproachInteractableComp();
	if (ApproachInteractableComp != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interacts with : %s"), *ApproachInteractableComp->GetName());
		IApproachInteract::Execute_Interact(ApproachInteractableComp);
	}
}

void AGravityCharacter::OnApproachInteractReleased()
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
void AGravityCharacter::OnClickInteract()
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

		// Try getting a component with a UClickInteract interface
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
						if (CanSwapGravity(CurrentClickFocus, NewClickFocus))
						{	
							// Flip gravity for both
							UE_LOG(LogTemp, Warning, TEXT("Swap gravity"));
							CurrentClickFocusComp->SetFlipGravity(!CurrentClickFocusComp->GetFlipGravity());
							NewClickFocusComp->SetFlipGravity(!NewClickFocusComp->GetFlipGravity());
						}
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

bool AGravityCharacter::CanSwapGravity(UActorComponent* Comp1, UActorComponent* Comp2)
{
	// Is the focus player?
	bool bIsFocusPlayer = false;
	if (Cast<AGravityCharacter>(Comp1->GetOwner()) != nullptr
		|| Cast<AGravityCharacter>(Comp2->GetOwner()) != nullptr)
	{
		bIsFocusPlayer = true;
	}

	// One of the focuses is player but does not have Relic1 power.
	if (bHasRelic1 == false && bIsFocusPlayer == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player does not have a Relic1 power"));
		ResetClickInteract(CurrentClickFocus);
		return false;
	}

	// both focuses are object but does not have Relic2 power
	if (bHasRelic2 == false && bIsFocusPlayer == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player does not have a Relic2 power"));
		ResetClickInteract(CurrentClickFocus);
		return false;
	}

	return true;
}

// End Interact
//////////////////////////////////////////////////////////////////////////
#pragma endregion Interaction