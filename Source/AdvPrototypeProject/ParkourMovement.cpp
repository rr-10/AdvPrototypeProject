// Fill out your copyright notice in the Description page of Project Settings.

#include "ParkourMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Math/UnrealMathUtility.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UParkourMovement::UParkourMovement()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UParkourMovement::BeginPlay()
{
	Super::BeginPlay();
	CurrentMovementMode = EParkourMovement::None;

	// ...

}

// Called every frame
void UParkourMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//CameraTick();
}

void UParkourMovement::Initialize(ACharacter* CharacterReference, UCharacterMovementComponent* MovementComponent)
{
	// Initialize Values 
	Character = CharacterReference;
	CharacterMovement = MovementComponent;
	DefaultGravity = MovementComponent->GravityScale;

	//Begin the wall running checks
	GetWorld()->GetTimerManager().SetTimer(ParkourUpdateTimer, this, &UParkourMovement::ParkourMovementUpdate, 0.02f, true);
}


void UParkourMovement::ToggleParkour()
{
	ShouldDoParkourMovement = !ShouldDoParkourMovement;
	WallRunEnd(1.0f);
	VerticalWallRunEnd(1.0f);
}

void UParkourMovement::ToggleParkourOn()
{
	ShouldDoParkourMovement = true;
}

void UParkourMovement::ToggleParkourOff()
{
	ShouldDoParkourMovement = false;
	WallRunEnd(1.0f);
	VerticalWallRunEnd(1.0f);
}

void UParkourMovement::JumpEvent()
{
	WallRunJump();
}

void UParkourMovement::LandEvent()
{
	WallRunEnd(0.0f);
	ResetSupression();
}

void UParkourMovement::CameraTick()
{
	/*if (WallRunningLeft)
	{
		CameraTilt(15.0f);
	}
	else if (WallRunningRight)
	{
		CameraTilt(-15.0f);
	}
	else
	{
		CameraTilt(0.0f);
	}*/
}

void UParkourMovement::ParkourMovementUpdate()
{
	if (!ShouldDoParkourMovement)
	{
		return;
	}


	WallRunUpdate();

	VerticalWallRunUpdate();
}

void UParkourMovement::LaunchSuppressionTimer(float Delay)
{
	MovementSupressed = true;
	GetWorld()->GetTimerManager().SetTimer(SuppressionTimer, this, &UParkourMovement::ResetSupression, Delay, false);
}

void UParkourMovement::VerticalWallRunUpdate()
{
	if (CanVerticalWallRun())
	{
		/// <summary>
		/// Create a capsule shape and check for collision in that area using SweepMultiByChannel
		/// </summary>
		/// 
		
	
		TArray<FHitResult> OutHits;

		// Calculate the eye level checking position
		FVector EyeLevel;
		FRotator DontNeed;
		Character->GetController()->GetActorEyesViewPoint(EyeLevel, DontNeed);
		FVector EyeLevelWithOffset = (EyeLevel + FVector{ 0.0f,0.0f,50.0f }) + Character->GetActorForwardVector() * 50.0f; // Moved forward and up relative to the player 

		// Calculate the feet level checking position
		float HalfHeightMinusMantleHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - MantleHeight;
		FVector FeetLevel = (Character->GetActorLocation() - FVector{ 0.0f,0.0f, HalfHeightMinusMantleHeight }) + Character->GetActorForwardVector() * 50.0f;

		FVector CapsuleExtents = { 0.0f, 20.0f, 10.0f }; // TODO : Set this to the correct values 

		FCollisionShape CollisionCapsule = FCollisionShape::MakeCapsule(CapsuleExtents);

		bool isHit = GetWorld()->SweepMultiByChannel(OutHits, EyeLevelWithOffset, FeetLevel, FQuat::Identity, ECC_WorldStatic, CollisionCapsule);

		if (isHit)
		{
			MantleTraceDistance = OutHits[0].Distance;

			if (CharacterMovement->IsWalkable(OutHits[0]))
			{
				// Perform the mantle 
				VerticalWallRunEnd(0.35f);
			}

			// Wall Run Vertically 
			VerticalWallRunMovement(FeetLevel);
		}
	}
	else if (!IsWallingRunning())
	{
		VerticalWallRunEnd(SupressionDelayLength);
	}
}

bool UParkourMovement::CanVerticalWallRun()
{
	float ForwardInput = FVector::DotProduct(Character->GetActorForwardVector(), CharacterMovement->GetLastInputVector());

	if (ForwardInput > 0.0f && CharacterMovement->IsFalling() && (CurrentMovementMode == EParkourMovement::None || CurrentMovementMode == EParkourMovement::VerticalWallRun))
	{
		// Conditions met to be able to mantle 
		return true;
	}
	return false;
}

void UParkourMovement::VerticalWallRunMovement(FVector Feet)
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("WallTrace")), false, GetOwner());
	TraceParams.bReturnPhysicalMaterial = false;

	FVector End = Feet + (Character->GetActorForwardVector() * 50.0f);

	bool isHit = GetWorld()->LineTraceSingleByChannel(HitResult, Feet, End, ECollisionChannel::ECC_WorldStatic, TraceParams);

	if (CanVerticalWallRun() && isHit)
	{
		WallRunNormal = HitResult.ImpactNormal;
		CurrentMovementMode = EParkourMovement::VerticalWallRun;

		// Stick to the wall and launch the player upwards 
		Character->LaunchCharacter(FVector{ WallRunNormal.X * -600.0f, WallRunNormal.Y * -600.0f, VerticalWallRunSpeed }, true, true);
	}
	else
	{
		VerticalWallRunEnd(0.35);
	}
}

void UParkourMovement::VerticalWallRunEnd(float ResetTime)
{
	CurrentMovementMode = EParkourMovement::None;
}

void UParkourMovement::WallRunUpdate()
{
	if (MovementSupressed || !CharacterMovement->IsFalling())
		return;

	FVector rightEndPoint = Character->GetActorLocation() + (Character->GetActorForwardVector() * -35.0f) + (Character->GetActorRightVector() * 75.0f);
	FVector leftEndPoint = Character->GetActorLocation() + (Character->GetActorForwardVector() * -35.0f) + (Character->GetActorRightVector() * -75.0f);

	if (WallRunMovement(Character->GetActorLocation(), rightEndPoint, -1.0f))
	{
		CurrentMovementMode = EParkourMovement::WallRunningRight;

		// Interpolate gravity scale 
		CharacterMovement->GravityScale = FMath::FInterpTo(CharacterMovement->GravityScale, WallRunTargetGravity, GetWorld()->DeltaTimeSeconds, 10.0f);
	}
	else if (WallRunMovement(Character->GetActorLocation(), leftEndPoint, 1.0f))
	{
		CurrentMovementMode = EParkourMovement::WallRunningLeft;

		// Interpolate gravity scale 
		CharacterMovement->GravityScale = FMath::FInterpTo(CharacterMovement->GravityScale, WallRunTargetGravity, GetWorld()->DeltaTimeSeconds, 10.0f);
	}
	else if (IsWallingRunning())
	{
		WallRunEnd(SupressionDelayLength);
	}
}

bool UParkourMovement::WallRunMovement(FVector Start, FVector End, float WallRunDirection)
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("WallTrace")), false, GetOwner());
	TraceParams.bReturnPhysicalMaterial = false;

	bool isHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, TraceParams);

	if (HitResult.bBlockingHit)
	{
		// Check that the hit is a valid wall 
		WallRunNormal = HitResult.Normal;

		TArray<UActorComponent*> ComponentResult = HitResult.GetActor()->GetComponentsByTag(UActorComponent::StaticClass(), FName{"WallRunning"});

		if ((WallRunNormal.Z > -0.52 && WallRunNormal.Z < 0.52) && (ComponentResult.Num() > 0))
		{
			// Stick to the wall 
			FVector PushToWallVector = WallRunNormal * (WallRunNormal - Character->GetActorLocation()).Size();
			Character->LaunchCharacter(PushToWallVector, false, false);

			// Move the player forward
			FVector WallUpCross = FVector::CrossProduct(WallRunNormal, { 0.0f,0.0f,1.0f });
			float Multi = WallRunSpeed * WallRunDirection;

			FVector ForwardMovement = WallUpCross * Multi;
			Character->LaunchCharacter(ForwardMovement, true, !WallRunGravityEnabled);

			return true;
		}
	}

	return false;
}

void UParkourMovement::WallRunJump()
{
	if (IsWallingRunning())
	{
		WallRunEnd(0.3f); // This value is set to a lower level of delay to allow for bouncing from wall to wall 

		//Launch the character off the wall
		FVector JumpVector = { WallRunJumpAwayDistance * WallRunNormal.X, WallRunJumpAwayDistance * WallRunNormal.Y, WallRunJumpHeight };
		Character->LaunchCharacter(JumpVector, false, true);
	}
}

void UParkourMovement::WallRunEnd(float Delay)
{
	if (IsWallingRunning())
	{
		CurrentMovementMode = EParkourMovement::None;
		CharacterMovement->GravityScale = DefaultGravity;

		// Start a timer that block parkour movement for a length of time 
		LaunchSuppressionTimer(Delay);
	}
}

void UParkourMovement::CameraTilt(float TargetRoll)
{
	UE_LOG(LogTemp, Warning, TEXT("Tilting Camera!"));
	FRotator CamRotate = Character->GetController()->GetControlRotation();

	CameraTargetRotation = { TargetRoll, CamRotate.Pitch, CamRotate.Yaw };

	Character->GetController()->SetControlRotation(FMath::RInterpTo(Character->GetController()->GetControlRotation(), CameraTargetRotation, GetWorld()->DeltaTimeSeconds, 10.0f));
}


void UParkourMovement::ResetSupression()
{
	MovementSupressed = false;
	GetWorld()->GetTimerManager().ClearTimer(SuppressionTimer);
}

bool UParkourMovement::IsWallingRunning()
{
	return (CurrentMovementMode == EParkourMovement::WallRunningLeft || CurrentMovementMode == EParkourMovement::WallRunningRight);
}

