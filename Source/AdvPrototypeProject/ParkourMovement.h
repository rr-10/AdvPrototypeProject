// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourMovement.generated.h"

class UCharacterMovementComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ADVPROTOTYPEPROJECT_API UParkourMovement : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UParkourMovement();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	UPROPERTY(EditAnywhere, Category = "Wall Running")
		bool WallRunGravityEnabled = false;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
		float WallRunTargetGravity = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
		float WallRunSpeed = 850.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
		float WallRunJumpHeight = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
		float WallRunJumpAwayDistance = 300.0f;
	

public:

	UFUNCTION(BlueprintCallable)
		void Initialize(ACharacter* CharacterReference, UCharacterMovementComponent* MovementComponent);

	UFUNCTION(BlueprintCallable)
		void JumpEvent();

	UFUNCTION(BlueprintCallable)
		void LandEvent();

private:
	UFUNCTION()
		void CameraTick();

	UFUNCTION()
		void WallRunUpdate();

	UFUNCTION()
		bool WallRunMovement(FVector Start, FVector End, float WallRunDirection);

	UFUNCTION()
		void WallRunJump();

	UFUNCTION()
		void WallRunEnd(float Delay);

	UFUNCTION()
		void CameraTilt(float TargetRoll);

	UFUNCTION()
		void SupressWallRun(float Delay);

	UFUNCTION()
		void ResetWallRunSupression();

private:
	ACharacter* Character;
	UCharacterMovementComponent* CharacterMovement;
	float DefaultGravity;

private:
	FTimerHandle WallRunUpdateTimer;
	FTimerHandle WallRunSuppressionTimer;
	FVector WallRunNormal;
	bool WallRunning = false;
	bool WallRunningRight = false;
	bool WallRunningLeft = false;
	bool WallRunSupressed = false;
	FRotator CameraTargetRotation;
};
