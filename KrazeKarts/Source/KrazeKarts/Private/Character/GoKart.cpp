// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();

	//NetUpdateFrequency = 1;	
}

void AGoKart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ReplicatedTransform);
}

FString GetRoleText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "Error";
	}
}


// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = MaxDrivingForce * Throttle * GetActorForwardVector();

	Force += GetAirResistance();

	Force += GetRollingResistance();

	FVector Accelerate = Force / Mass;

	velocity = velocity + Accelerate * DeltaTime;

	ApplyRotaion(DeltaTime);

	UpdateLocationFromVelocity(DeltaTime);

	if (HasAuthority())
	{
		ReplicatedTransform = GetActorTransform();	
	}
	else {
		SetActorTransform(ReplicatedTransform);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetRoleText(Role), this, FColor::White, DeltaTime);
}

//void AGoKart::OnRep_ReplicatedTransform()
//{
//	SetActorTransform(ReplicatedTransform);
//}

void AGoKart::ApplyRotaion(float DeltaTime)
{
	float DeltaRotation = FVector::DotProduct(GetActorForwardVector(), velocity) * DeltaTime;

	float RotationAngle = DeltaRotation / MinTurningRadius * SteeringtThrow;

	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

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

FVector AGoKart::GetAirResistance()
{
	return -Dragcoefficient * velocity.SizeSquared() * velocity.GetSafeNormal();
}

FVector AGoKart::GetRollingResistance()
{
	float GravityAccelerate = -GetWorld()->GetGravityZ() / 100;
	float Normalforce = Mass * GravityAccelerate;
	return -velocity.GetSafeNormal() * Normalforce * RollingResistanceCoefficient;
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
	Server_MoveForward(val);
}

void AGoKart::MoveRight(float val)
{
	SteeringtThrow = val;
	Server_MoveRight(val);
}

void AGoKart::Server_MoveForward_Implementation(float val)
{
	Throttle = val;
}

bool AGoKart::Server_MoveForward_Validate(float val)
{
	return FMath::Abs(val) <= 1;
}


void AGoKart::Server_MoveRight_Implementation(float val)
{
	SteeringtThrow = val;
}

bool AGoKart::Server_MoveRight_Validate(float val)
{
	return FMath::Abs(val) <= 1;
}
