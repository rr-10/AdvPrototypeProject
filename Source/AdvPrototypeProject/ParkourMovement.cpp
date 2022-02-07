// Fill out your copyright notice in the Description page of Project Settings.

#include "ParkourMovement.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Math/UnrealMathUtility.h"

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
	UE_LOG(LogTemp, Warning, TEXT("Setting Values!"));

	// Initialize Values 
	Character = CharacterReference;
	CharacterMovement = MovementComponent;
	DefaultGravity = MovementComponent->GravityScale;

	//Begin the wall running checks
	GetWorld()->GetTimerManager().SetTimer(WallRunUpdateTimer, this, &UParkourMovement::WallRunUpdate, 0.02f, true);
}

void UParkourMovement::JumpEvent()
{
	WallRunJump();
}

void UParkourMovement::LandEvent()
{
	WallRunEnd(0.0f);
	ResetWallRunSupression();
}

void UParkourMovement::CameraTick()
{
	if (WallRunningLeft)
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
	}
}

void UParkourMovement::WallRunUpdate()
{
	if (WallRunSupressed)
		return;

	FVector rightEndPoint = Character->GetActorLocation() + (Character->GetActorForwardVector() * -35.0f) + (Character->GetActorRightVector() * 75.0f);
	FVector leftEndPoint = Character->GetActorLocation() + (Character->GetActorForwardVector() * -35.0f) + (Character->GetActorRightVector() * -75.0f);

	if (WallRunMovement(Character->GetActorLocation(), rightEndPoint, -1.0f))
	{
		WallRunning = true;
		WallRunningRight = true;
		WallRunningLeft = false;

		// Interpolate gravity scale 
		CharacterMovement->GravityScale = FMath::FInterpTo(CharacterMovement->GravityScale, WallRunTargetGravity, GetWorld()->DeltaTimeSeconds, 10.0f);
	}

	else if (WallRunMovement(Character->GetActorLocation(), leftEndPoint, 1.0f))
	{
		WallRunning = true;
		WallRunningRight = false;
		WallRunningLeft = true;

		// Interpolate gravity scale 
		CharacterMovement->GravityScale = FMath::FInterpTo(CharacterMovement->GravityScale, WallRunTargetGravity, GetWorld()->DeltaTimeSeconds, 10.0f);
	}
	else if (WallRunning)
	{
		WallRunEnd(1.0f);
	}
}


bool UParkourMovement::WallRunMovement(FVector Start, FVector End, float WallRunDirection)
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("WallTrace")), false, GetOwner());
	TraceParams.bReturnPhysicalMaterial = false;

	bool isHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, TraceParams);

	if (HitResult.bBlockingHit && CharacterMovement->IsFalling())
	{
		// Check that the hit is a valid wall 
		WallRunNormal = HitResult.Normal;

		if (WallRunNormal.Z > -0.52 && WallRunNormal.Z < 0.52)
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
	if (WallRunning)
	{
		WallRunEnd(0.3f);

		//Launch the character off the wall
		FVector JumpVector = { WallRunJumpAwayDistance * WallRunNormal.X, WallRunJumpAwayDistance * WallRunNormal.Y, WallRunJumpHeight };
		Character->LaunchCharacter(JumpVector, false, true);
	}
}

void UParkourMovement::WallRunEnd(float Delay)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to end!"));
	if (WallRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wall Run End!"));
		WallRunning = false;
		WallRunningLeft = false;
		WallRunningRight = false;
		CharacterMovement->GravityScale = DefaultGravity;

		SupressWallRun(Delay); // TODO : Promote to variable 
	}
}

void UParkourMovement::CameraTilt(float TargetRoll)
{
	UE_LOG(LogTemp, Warning, TEXT("Tilting Camera!"));
	FRotator CamRotate = Character->GetController()->GetControlRotation();

	CameraTargetRotation = { TargetRoll, CamRotate.Pitch, CamRotate.Yaw };

	Character->GetController()->SetControlRotation(FMath::RInterpTo(Character->GetController()->GetControlRotation(), CameraTargetRotation, GetWorld()->DeltaTimeSeconds, 10.0f));
}

void UParkourMovement::SupressWallRun(float Delay)
{
	WallRunSupressed = true;
	GetWorld()->GetTimerManager().SetTimer(WallRunSuppressionTimer, this, &UParkourMovement::ResetWallRunSupression, Delay, false);
}

void UParkourMovement::ResetWallRunSupression()
{
	WallRunSupressed = false;
	GetWorld()->GetTimerManager().ClearTimer(WallRunSuppressionTimer);
}

