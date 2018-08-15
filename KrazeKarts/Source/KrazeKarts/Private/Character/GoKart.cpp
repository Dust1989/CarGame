// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"

FString GetRoleText(ENetRole Role);

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

	NetUpdateFrequency = 1;	
}

void AGoKart::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ServerState);
}


// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	if (IsLocallyControlled())
	{
		FGoKartMove Move = CreateMove(DeltaTime);
		
		if (!HasAuthority())
		{
			UnacknowledgeeMoves.Add(Move);

			SimulateMove(Move);
			UE_LOG(LogTemp, Warning, TEXT("Queue length: %d"), UnacknowledgeeMoves.Num());
		}
		
		Server_SendMove(Move);
	}
	*/

	if (Role == ROLE_AutonomousProxy)
	{
		FGoKartMove Move = CreateMove(DeltaTime);

		UnacknowledgeeMoves.Add(Move);

		SimulateMove(Move);
		UE_LOG(LogTemp, Warning, TEXT("Queue length: %d"), UnacknowledgeeMoves.Num());

		Server_SendMove(Move);
	}

	if (Role == ROLE_Authority && GetRemoteRole() == ROLE_SimulatedProxy)
	{
		FGoKartMove Move = CreateMove(DeltaTime);
		Server_SendMove(Move);
	}

	if (Role == ROLE_SimulatedProxy)
	{
		SimulateMove(ServerState.LastMove);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetRoleText(Role), this, FColor::White, DeltaTime);
}



void AGoKart::OnRep_ServerState()
{
	SetActorTransform(ServerState.Transform);
	Velocity = ServerState.Velocity;

	ClearAcknowledgetMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : UnacknowledgeeMoves)
	{
		SimulateMove(Move);
	}
}

FGoKartMove AGoKart::CreateMove(float DelatTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DelatTime;
	Move.Throttle = Throttle;
	Move.SteeringThrow = SteeringThrow;
	Move.Time = GetWorld()->TimeSeconds;

	return Move;
}

void AGoKart::ClearAcknowledgetMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;

	for (const FGoKartMove& Move : UnacknowledgeeMoves)
	{
		if (Move.Time > LastMove.Time)
		{
			NewMoves.Add(Move);
		}
	}

	UnacknowledgeeMoves = NewMoves;
}

void AGoKart::SimulateMove(const FGoKartMove& move)
{
	FVector Force = MaxDrivingForce * move.Throttle * GetActorForwardVector();

	Force += GetAirResistance();

	Force += GetRollingResistance();

	FVector Accelerate = Force / Mass;

	Velocity = Velocity + Accelerate * move.DeltaTime;

	ApplyRotaion(move.DeltaTime, move.SteeringThrow);

	UpdateLocationFromVelocity(move.DeltaTime);
}

void AGoKart::ApplyRotaion(float DeltaTime, float SteeringThrow)
{
	float DeltaRotation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;

	float RotationAngle = DeltaRotation / MinTurningRadius * SteeringThrow;

	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

	AddActorWorldRotation(RotationDelta);

	Velocity = RotationDelta.RotateVector(Velocity);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Magenta, FString::Printf(TEXT("val = %s"), *Translation.ToString()));

	FHitResult hit;
	AddActorWorldOffset(Translation, true, &hit);
	if (hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

FVector AGoKart::GetAirResistance()
{
	return -Dragcoefficient * Velocity.SizeSquared() * Velocity.GetSafeNormal();
}

FVector AGoKart::GetRollingResistance()
{
	float GravityAccelerate = -GetWorld()->GetGravityZ() / 100;
	float Normalforce = Mass * GravityAccelerate;
	return -Velocity.GetSafeNormal() * Normalforce * RollingResistanceCoefficient;
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
	SteeringThrow = val;
}

void AGoKart::Server_SendMove_Implementation(FGoKartMove move)
{
	SimulateMove(move);
	
	ServerState.LastMove = move;
	ServerState.Transform = GetActorTransform();
	ServerState.Velocity = Velocity;
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove move)
{
	return true; //TODO: make a better validation
}



// Tool Function
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