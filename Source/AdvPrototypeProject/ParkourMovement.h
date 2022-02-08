// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourMovement.generated.h"

class UCharacterMovementComponent;

UENUM(BlueprintType)
enum class EParkourMovement : uint8
{
	None UMETA(DisplayName = "None"),
	WallRunningLeft UMETA(DisplayName = "WallRunningLeft"),
	WallRunningRight UMETA(DisplayName = "WallRunningRight"),
	VerticalWallRun UMETA(DisplayName = "VerticalWallRun"),
	LedgeGrabbing UMETA(DisplayName = "LedgeGrabbing"),
	Mantle UMETA(DisplayName = "Mantle"),
	Slide UMETA(DisplayName = "Slide"),
	Sprint UMETA(DisplayName = "Sprint")
};

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
	// General Options 
	UPROPERTY(EditAnywhere, Category = "General")
		float SupressionDelayLength = 1.0f;

	// Wall Running Options 
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

	// Wall Climbing Options 
	UPROPERTY(EditAnywhere, Category = "Wall Climbing")
		float MaximumWallClimbHeight = 120.0f;
	UPROPERTY(EditAnywhere, Category = "Wall Climbing")
		float VerticalWallRunSpeed = 300.0f;

	// Mantle Options 
	UPROPERTY(EditAnywhere, Category = "Mantle")
		float MantleHeight = 40.0f;


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
		void LaunchSuppressionTimer(float Delay);

	UFUNCTION()
		void ResetSupression();

private:
	// Vertical Wall Run Functions
	UFUNCTION()
		void VerticalWallRunUpdate();

	UFUNCTION()
		bool CanVerticalWallRun();

	UFUNCTION()
		void VerticalWallRunMovement(FVector Feet);

	UFUNCTION()
		void VerticalWallRunEnd(float ResetTime);

private:
	// Wall Running Functions
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
		bool IsWallingRunning();

private:
	ACharacter* Character;
	UCharacterMovementComponent* CharacterMovement;
	float DefaultGravity;

private:
	UPROPERTY(VisibleAnywhere, Category = ParkourMovement)
		TEnumAsByte<EParkourMovement> CurrentMovementMode;

	FTimerHandle WallRunUpdateTimer;
	FTimerHandle VerticalWallRunTimer;
	FTimerHandle SuppressionTimer;
	
	bool MovementSupressed = false;

	FRotator CameraTargetRotation;

	float MantleTraceDistance;
	FVector WallRunNormal;
};
