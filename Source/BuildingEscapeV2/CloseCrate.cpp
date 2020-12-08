// Fill out your copyright notice in the Description page of Project Settings.

#include "CloseCrate.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UCloseCrate::UCloseCrate()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCloseCrate::BeginPlay()
{
	Super::BeginPlay();

	TargetZ = GetOwner()->GetActorLocation().Z + OpenHight;
	CloseZ = GetOwner()->GetActorLocation().Z;

	FindPressurePlate();

	// Allow player to active pressure plate
	if(!OpenViaMass)
		ActorThatOpen = GetWorld()->GetFirstPlayerController()->GetPawn();

}

void UCloseCrate::FindPressurePlate()
{
	// Check do actor have pressure plate
	if(PressurePlate == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("The actor %s don't have pressure plate!"), *GetOwner()->GetName())
	}
}

// Called every frame
void UCloseCrate::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
		OpenCrate(DeltaTime);
		CrateLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		if (GetWorld()->GetTimeSeconds() - CrateLastOpened > CrateCloseDelay)
		{
		CloseCrate(DeltaTime);
		}
	}
}

// Open door
void UCloseCrate::OpenCrate(float DeltaTime)
{
	CurrentZ = GetOwner()->GetActorLocation().Z;
	FVector EndHight = GetOwner()->GetActorLocation();
	EndHight.Z = TargetZ;

	EndHight.Z = FMath::FInterpTo(CurrentZ, TargetZ, DeltaTime, CrateOpenSpeed);
	GetOwner()->SetActorLocation(EndHight);
}

// Close door
void UCloseCrate::CloseCrate(float DeltaTime)
{
	CurrentZ = GetOwner()->GetActorLocation().Z;
	FVector EndHight = GetOwner()->GetActorLocation();
	EndHight.Z = CloseZ;

	EndHight.Z = FMath::FInterpTo(CurrentZ, CloseZ, DeltaTime, CrateCloseSpeed);
	GetOwner()->SetActorLocation(EndHight);
}

float UCloseCrate::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	TArray<AActor*> OverlappingActors ;

	if (PressurePlate == nullptr) {return TotalMass;}
	PressurePlate->GetOverlappingActors(OverlappingActors); 
	for(AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		// UE_LOG(LogTemp, Warning, TEXT("The actor %s is overlapping"), *Actor->FindComponentByClass<UPrimitiveComponent>()->GetName())
	}
	return TotalMass;
}