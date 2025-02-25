// Fill out your copyright notice in the Description page of Project Settings.


#include "TwinBlastPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Input
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Animation
#include "PlayerAnimInstance.h"

// Sets default values
ATwinBlastPlayer::ATwinBlastPlayer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 캐릭터 회전 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATwinBlastPlayer::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->OnMontageEnded.AddDynamic(this, &ATwinBlastPlayer::OnAttackMontageEnded);
}

// Called every frame
void ATwinBlastPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATwinBlastPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATwinBlastPlayer::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATwinBlastPlayer::Look);

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATwinBlastPlayer::Fire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATwinBlastPlayer::StopFire);
	}
	else
	{
		UE_LOG(LogTemp, Error , TEXT("'%s' Failed"), *GetNameSafe(this));
	}
}

void ATwinBlastPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ATwinBlastPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ATwinBlastPlayer::Fire()
{
	if (IsValid(AnimInstance))
	{
		AnimInstance->PlayMontage();
	}
}

void ATwinBlastPlayer::StopFire()
{

}

void ATwinBlastPlayer::OnAttackMontageEnded(UAnimMontage* Montage, bool Interrupted)
{
	float AttackRange = 10000.f;

	FHitResult HitResult;

	FVector Center = GetActorLocation();
	FVector Forwad = Center + GetActorForwardVector() * AttackRange;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	bool Result = GetWorld()->LineTraceSingleByChannel
	(
		OUT HitResult,
		Center,
		Forwad,
		ECollisionChannel::ECC_GameTraceChannel1,
		params
	);

	if (Result)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Not Hit"));
	}

	DrawDebugLine(GetWorld(), Center, HitResult.Location, FColor::Green);
}
