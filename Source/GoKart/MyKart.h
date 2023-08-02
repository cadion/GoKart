// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyKart.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float SteeringThrow;

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
	FTransform Transform;
	
	UPROPERTY()
	FVector Velocity;
	
	FGoKartMove LastMoves;
};


UCLASS()
class GOKART_API AMyKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector GetAirResistance() const;
	FVector GetRollingResistance() const;
	
	void ApplyRotation(float DeltaTime);
	
	void UpdateLocationFromVelocity(float DeltaTime);

	// The mass of the car (g).
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	// the force applied to the car when the throttle is fully down (N)
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// the number of degrees rotated per second at full control throw (degrees/s)
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	void MoveForward(float Value);
	void MoveRight(float Value);
	
	UFUNCTION(server, reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	FVector Velocity;
	
	UFUNCTION()
	void OnRep_ServerState();

	UPROPERTY(Replicated)
	float Throttle;

	UPROPERTY(Replicated)
	float SteeringThrow;
	
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;
};
