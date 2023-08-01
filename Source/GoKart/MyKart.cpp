// Fill out your copyright notice in the Description page of Project Settings.


#include "MyKart.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AMyKart::AMyKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AMyKart::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		NetUpdateFrequency = 1;
	}
}

void AMyKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyKart, ReplicatedTransform);
	DOREPLIFETIME(AMyKart, Velocity);
	DOREPLIFETIME(AMyKart, Throttle);
	DOREPLIFETIME(AMyKart, SteeringThrow);
}


FString GetEnumText(ENetRole Role)
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
void AMyKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;
	
	Velocity = Velocity + Acceleration * DeltaTime;
	

	ApplyRotation(DeltaTime);

	UpdateLocationFromVelocity(DeltaTime);

	if(HasAuthority())
	{
		ReplicatedTransform = GetActorTransform();
	}
	
	ENetRole tempRole = GetLocalRole();
	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(tempRole), this, FColor::White, DeltaTime);
}


void AMyKart::OnRep_ReplicatedTransform()
{
	SetActorTransform(ReplicatedTransform);
}

// Called to bind functionality to input
void AMyKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyKart::MoveRight);
}

FVector AMyKart::GetAirResistance() const
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AMyKart::GetRollingResistance() const
{
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	float NormalForce = Mass * AccelerationDueToGravity;
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void AMyKart::ApplyRotation(float DeltaTime)
{
	float DeltaLocation = Velocity.Dot(GetActorForwardVector()) * DeltaTime;
	float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrow;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

	Velocity = RotationDelta.RotateVector(Velocity);

	AddActorWorldRotation(RotationDelta);
}

void AMyKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;

	FHitResult Hit;
	AddActorWorldOffset(Translation, true, &Hit);
	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

void AMyKart::MoveForward(float Value)
{
	Throttle = Value;
	Server_MoveForward(Value);
}

void AMyKart::MoveRight(float Value)
{
	SteeringThrow = Value;
	Server_MoveRight(Value);
}


void AMyKart::Server_MoveForward_Implementation(float Value)
{
	Throttle = Value;
}

bool AMyKart::Server_MoveForward_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}


void AMyKart::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool AMyKart::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}


