// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityPlayerCharacter.h"
#include "GravityMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractionInterface.h"
#include "GravityCube.h"
#include "GravitySwappable.h"
#include "ClickInteract.h"	
#include "DrawDebugHelpers.h"

AGravityPlayerCharacter::AGravityPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Tick is needed for gravity functionality
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

void AGravityPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Calculate three vector to make the rotation space for camera
	const FVector ForwardVector{ -1.0f, 0.0f, 0.0f };
	auto UpVector = UKismetMathLibrary::GetDirectionUnitVector(GravityPoint, GetActorLocation());
	auto RightVector = FVector::CrossProduct(UpVector, ForwardVector);

	// Calculate the rotation and set rotation
	auto TargetRotation = UKismetMathLibrary::MakeRotationFromAxes(ForwardVector, RightVector, UpVector);
	TargetRotation = FMath::RInterpTo(CameraBoom->GetComponentRotation(), TargetRotation, DeltaTime, 15);
	CameraBoom->SetWorldRotation(TargetRotation);

	// Interaction
	if (Interactable != nullptr)
	{
		Interactable->HideInteractionWidget();
	}
	Interactable = GetClosestInteracterbleActor();
	if (Interactable != nullptr)
	{
		Interactable->ShowInteractionWidget();
	}
}

void AGravityPlayerCharacter::MoveRight(float Val)
{
	// Add movement with consideration to the direction of camera
	FVector localMove{ 0.0f, 1.0f, 0.0f };
	auto cameraRotation = CameraBoom->GetComponentRotation().Quaternion();
	auto result = cameraRotation * localMove;
	AddMovementInput({ 0.f,  result.Y, result.Z }, Val);
}

//////////////////////////////////////////////////////////////////////////
// Interact
void AGravityPlayerCharacter::OnInteract()
{
	if (Interactable != nullptr)
	{
		Interactable->Interact();
	}
}

void AGravityPlayerCharacter::OnClick()
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
				CurrentClickFocus->ResetClickInteract();
				CurrentClickFocus = nullptr;
			}
			return;
		}

		// GravitySwap
		TScriptInterface<IGravitySwappable> GravitySwappable = TScriptInterface<IGravitySwappable>(HitActor);
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
				if ((FirstFocus->CanSwap(GravitySwappable)) == true)
				{
					FirstFocus->SwapGravity(GravitySwappable);
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
		IClickInteract* ClickInteractable = Cast<IClickInteract>(HitActor);
		if (ClickInteractable != nullptr)
		{
			// When player clicks the second clickable object
			if (CurrentClickFocus != nullptr)
			{
				CurrentClickFocus->ResetClickInteract();
				ClickInteractable->ResetClickInteract();
				CurrentClickFocus = nullptr;
				return;
			}

			CurrentClickFocus = ClickInteractable;
			CurrentClickFocus->ClickInteract();
		}
		else
		{
			CurrentClickFocus->ResetClickInteract();
			CurrentClickFocus = nullptr;
		}

		IInteractionInterface* InteractableActor = Cast<IInteractionInterface>(HitActor);
		if (InteractableActor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Interactable object is clicked"));
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
			CurrentClickFocus->ResetClickInteract();
			CurrentClickFocus = nullptr;
		}
	}
}

// End Interact
//////////////////////////////////////////////////////////////////////////

void AGravityPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGravityPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &AGravityPlayerCharacter::OnInteract);
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Released, this, &AGravityPlayerCharacter::OnClick);

}

IInteractionInterface* AGravityPlayerCharacter::GetClosestInteracterbleActor()
{
	TArray<AActor*> OverlappingActors;
	InteractBox->GetOverlappingActors(OverlappingActors);
	if (OverlappingActors.Num() == 0)
	{
		return nullptr;
	}

	if (Interactable != nullptr)
	{
		Interactable->HideInteractionWidget();
		Interactable = nullptr;
	}

	// Get the closest actor among the Overlapping actors
	AActor* ClosestActor = OverlappingActors[0];
	for (auto CurrentActor : OverlappingActors)
	{
		if (GetDistanceTo(CurrentActor) < GetDistanceTo(ClosestActor))
		{
			ClosestActor = CurrentActor;
		}
	}

	return Cast<IInteractionInterface>(ClosestActor);

}
