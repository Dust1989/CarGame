// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

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
	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotaion(float DeltaTime);

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

	float SteeringtThrow;

	FVector velocity;

	float Throttle;
	
	void MoveForward(float val);

	void MoveRight(float val);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float val);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float val);

	/*UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTransform)
	FTransform ReplicatedTransform;*/

	UPROPERTY(Replicated)
	FTransform ReplicatedTransform; 

	/*UFUNCTION()
	void OnRep_ReplicatedTransform();*/
};
