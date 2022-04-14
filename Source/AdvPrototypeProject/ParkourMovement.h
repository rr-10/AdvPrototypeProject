// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ParkourMovement.generated.h"

class UCharacterMovementComponent;

UENUM(BlueprintType, Category = "Parkour")
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


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartedWallRunning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartedWallClimbing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStoppedWallRunning);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStoppedWallClimbing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMantle);

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
	
	UPROPERTY(BlueprintAssignable)
		FStartedWallRunning OnStartedWallRunning;
	UPROPERTY(BlueprintAssignable)
		FStoppedWallRunning OnStoppedWallRunning;
	UPROPERTY(BlueprintAssignable)
		FStartedWallClimbing OnStartedWallClimbing;
	UPROPERTY(BlueprintAssignable)
		FStoppedWallClimbing OnStoppedWallClimbing;
	UPROPERTY(BlueprintAssignable)
		FMantle OnMantle;

private:
	UFUNCTION()
		void TryBroadcastStartWallRun();
	UFUNCTION()
		void TryBroadcastStartWallClimb();
public:
	//Toggle 
	UPROPERTY(EditAnywhere, Category = "Toggle")
		bool ShouldDoParkourMovement = true;

	// General Options 
	UPROPERTY(EditAnywhere, Category = "General")
		float SuppressionDelayLength = 1.0f;

	// Wall Running Options 
	UPROPERTY(EditAnywhere, Category = "Wall Running")
		bool WallRunGravityEnabled = true;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
		float WallRunTargetGravity = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
		float WallRunSpeed = 850.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
		float WallRunJumpHeight = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Running")
		float WallRunJumpAwayDistance = 400.0f;

	// Wall Climbing Options 
	UPROPERTY(EditAnywhere, Category = "Wall Climbing")
		float MaximumWallClimbHeight = 600.0f;
	UPROPERTY(EditAnywhere, Category = "Wall Climbing")
		float VerticalWallRunSpeed = 300.0f;

	// Mantle Options 
	UPROPERTY(EditAnywhere, Category = "Mantle")
		float MantleHeight = 70.0f;

public:

	UFUNCTION(BlueprintCallable)
		void Initialize(ACharacter* CharacterReference, UCharacterMovementComponent* MovementComponent);

	UFUNCTION(BlueprintCallable)
		void JumpEvent();

	UFUNCTION(BlueprintCallable)
		void LandEvent();

	UFUNCTION(BlueprintCallable)
		void ToggleParkour();
	UFUNCTION(BlueprintCallable)
		void ToggleParkourOn();
	UFUNCTION(BlueprintCallable)
		void ToggleParkourOff();
private:
	UFUNCTION()
		void CameraTick();

	UFUNCTION()
		void ParkourMovementUpdate();

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
		void VerticalWallRunJump();

	UFUNCTION()
		void VerticalWallRunEnd(float ResetTime);
	UFUNCTION()
		void PerformWallMantle(FVector Feet, FVector MantlePosition);

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
		EParkourMovement CurrentMovementMode;

	FTimerHandle ParkourUpdateTimer;
	FTimerHandle SuppressionTimer;
	
	bool MovementSupressed = false;

	FRotator CameraTargetRotation;

	FVector VerticalRunStartPosition;

	float MantleTraceDistance;
	FVector WallRunNormal;
};
