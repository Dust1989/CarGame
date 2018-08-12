// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = MaxDrivingForce * Throttle * GetActorForwardVector();

	Force += GetResistance();

	FVector Accelerate = Force / Mass;

	velocity = velocity + Accelerate * DeltaTime;

	ApplyRotaion(DeltaTime);

	UpdateLocationFromVelocity(DeltaTime);


}

void AGoKart::ApplyRotaion(float DeltaTime)
{
	float rotateAngle = MaxRotateAngle * SteeringtThrow * DeltaTime;

	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(rotateAngle));

	AddActorWorldRotation(RotationDelta);

	velocity = RotationDelta.RotateVector(velocity);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = velocity * 100 * DeltaTime;

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, FString::Printf(TEXT("val = %s"), *Translation.ToString()));

	FHitResult hit;
	AddActorWorldOffset(Translation, true, &hit);
	if (hit.IsValidBlockingHit())
	{
		velocity = FVector::ZeroVector;
	}
}

FVector AGoKart::GetResistance()
{
	return -Dragcoefficient * velocity.SizeSquared() * velocity.GetSafeNormal();
}


// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);

}


void AGoKart::MoveForward(float val)
{
	Throttle = val;
}


void AGoKart::MoveRight(float val)
{
	SteeringtThrow = val;
}