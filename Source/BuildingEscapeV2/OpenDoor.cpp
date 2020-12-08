// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	// Sets The start end end Yaw's
	TargetYaw = GetOwner()->GetActorRotation().Yaw + OpenAngle;
	CloseYaw = GetOwner()->GetActorRotation().Yaw;

	FindPressurePlate();

	// Allow player to active pressure plate
	if(!OpenViaMass)
		ActorThatOpen = GetWorld()->GetFirstPlayerController()->GetPawn();

	FindAudioComponent();
}

void UOpenDoor::FindPressurePlate()
{
	// Check do actor have pressure plate
	if(PressurePlate == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("The actor %s don't have pressure plate!"), *GetOwner()->GetName())
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// if pressure plate is activated open door, else close door
	// (PressurePlate->IsOverlappingActor(ActorThatOpen))
	bool MassOrPlayer;
	if (PressurePlate == nullptr) {return;}
	if(OpenViaMass)
	{
		MassOrPlayer = TotalMassOfActors() > MassToOpen;
	}
	else
	{
		MassOrPlayer = PressurePlate->IsOverlappingActor(ActorThatOpen);
	}
	
	if (PressurePlate &&  MassOrPlayer)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
		CloseDoor(DeltaTime);
		}
	}
}

// Find audio component
void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	if(!AudioComponent)
		UE_LOG(LogTemp, Warning, TEXT("%s missing audio component!"), *GetOwner()->GetName());
}

// Open door
void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	FRotator EndRotation (0.f, TargetYaw, 0.f);

	EndRotation.Yaw = FMath::FInterpTo(CurrentYaw, TargetYaw, DeltaTime, DoorOpenSpeed);
	GetOwner()->SetActorRotation(EndRotation);

	CloseDoorSound = false;
	if (AudioComponent == nullptr) {return;}
	if (!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;	
	}
}


// Close door
void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	FRotator EndRotation (0.f, CloseYaw, 0.f);

	EndRotation.Yaw = FMath::FInterpTo(CurrentYaw, CloseYaw, DeltaTime, DoorCloseSpeed);
	GetOwner()->SetActorRotation(EndRotation);

	OpenDoorSound = false;
	if (AudioComponent == nullptr) {return;}
	if (!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;
	}

}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors ;

	if (PressurePlate == nullptr) {return TotalMass;}
	PressurePlate->GetOverlappingActors(OverlappingActors); 
	for(AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}