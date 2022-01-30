// Fill out your copyright notice in the Description page of Project Settings.


#include "CX_Character.h"

#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACX_Character::ACX_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACX_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACX_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// Called to bind functionality to input
void ACX_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACX_Character::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACX_Character::StopSprinting);

	PlayerInputComponent->BindAxis("move_forward", this, &ACX_Character::move_forward);
	PlayerInputComponent->BindAxis("move_side", this, &ACX_Character::move_side);

	PlayerInputComponent->BindAxis("look_vertical", this, &ACX_Character::look_vertical);
	PlayerInputComponent->BindAxis("look_horizontal", this, &ACX_Character::look_horizontal);
}

void ACX_Character::move_forward(float Value)
{
	if(Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACX_Character::move_side(float Value)
{
	if(Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACX_Character::look_vertical(float Axis)
{
	AddControllerPitchInput(Axis * Sensitivity);
}

void ACX_Character::look_horizontal(float Axis)
{
	AddControllerYawInput(Axis * Sensitivity);
}

void ACX_Character::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void ACX_Character::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

