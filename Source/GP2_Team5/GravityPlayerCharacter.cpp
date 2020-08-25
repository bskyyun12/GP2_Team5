// Fill out your copyright notice in the Description page of Project Settings.

#include "GravityPlayerCharacter.h"
#include "GravityMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

AGravityPlayerCharacter::AGravityPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGravityMovementComponent>(ACharacter::CharacterMovementComponentName))
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
}

void AGravityPlayerCharacter::MoveRight(float Val)
{
	// Add movement with consideration to the direction of camera
	FVector localMove{ 0.0f, 1.0f, 0.0f };
	auto cameraRotation = CameraBoom->GetComponentRotation().Quaternion();
	auto result = cameraRotation * localMove;
	AddMovementInput({ 0.f,  result.Y, result.Z }, Val);
}


void AGravityPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGravityPlayerCharacter::MoveRight);

}
