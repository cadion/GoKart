// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyKart.generated.h"

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
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);

	UPROPERTY(Replicated)
	FVector Velocity;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTransform)
	FTransform ReplicatedTransform;

	UFUNCTION()
	void OnRep_ReplicatedTransform();
	
	float Throttle;
	float SteeringThrow;

	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;
};
