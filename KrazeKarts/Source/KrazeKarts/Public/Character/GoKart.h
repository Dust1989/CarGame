// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"


USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FGoKartMove LastMove;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;
	
};


UCLASS()
class KRAZEKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	

private:
	FGoKartMove CreateMove(float DeltaTime);

	void ClearAcknowledgetMoves(FGoKartMove LastMove);

	void SimulateMove(const FGoKartMove& move);

	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotaion(float DeltaTime, float SteeringThrow);

	FVector GetAirResistance();

	FVector GetRollingResistance();

	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	UPROPERTY(EditAnywhere)
	float Dragcoefficient = 16;

	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;
	
	void MoveForward(float val);

	void MoveRight(float val);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove move);

	// The ServerState replicated with function --> OnRep_ServerState  
	// frequency is 1 second set by "NetUpdateFrequency = 1" in the BeginPlay
	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	FVector Velocity;

	UFUNCTION()
	void OnRep_ServerState();


	float SteeringThrow;
	float Throttle;

	TArray<FGoKartMove> UnacknowledgeeMoves;
};
