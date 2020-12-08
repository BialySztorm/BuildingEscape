// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetUpInputComponent();
}

//Checking for physics handle component
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle == nullptr)
		UE_LOG(LogTemp, Error, TEXT("ERROR with PhysicsHandle on %s"), *GetOwner()->GetName());
}

//SetUp physics handle input component
void UGrabber::SetUpInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Input Component found %S"), *GetOwner()->GetName());
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

//Grabs items
void UGrabber::Grab()
{
	// UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed"));

	// Gets grabbing item vector
	FVector EyeVector;
	FRotator EyeRotator;
	GetWorld()-> GetFirstPlayerController()->GetPlayerViewPoint(EyeVector, EyeRotator);

	FVector LineTrace = EyeVector + EyeRotator.Vector() * Reach;

	// Gets grabing item ID
	FHitResult Hit = GetFirstPhisycsBodyInReach();
	AActor* ActorHit = Hit.GetActor();
	UPrimitiveComponent* ComponentToGrab = Hit.GetComponent();

	// Grab the item you know the ID before (59)
	if( ActorHit)
	{	
		if (PhysicsHandle == nullptr){return;}
		// UE_LOG(LogTemp, Warning, TEXT("You Hit %s"), *(ActorHit->GetName()) );
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			LineTrace
		);
	}

}

//Gets first item in reach name
FHitResult UGrabber::GetFirstPhisycsBodyInReach() const
{
	// PlayerVievport
	FVector EyeVector;
	FRotator EyeRotator;
	GetWorld()-> GetFirstPlayerController()->GetPlayerViewPoint(EyeVector, EyeRotator);

	FVector LineTrace = EyeVector + EyeRotator.Vector() * Reach;

	FHitResult Hit;
	FCollisionQueryParams TraceParams{FName(TEXT("")), false, GetOwner()};
	// Ray-cast from distance (Reach)
	GetWorld()->LineTraceSingleByObjectType
	(
		Hit,
		EyeVector,
		LineTrace,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;
}

//Release item
void UGrabber::Release()
{
	if (PhysicsHandle == nullptr){return;}
	PhysicsHandle->ReleaseComponent();
	// UE_LOG(LogTemp, Warning, TEXT("Grabber Released"));
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Items and DebugLine Vector
	FVector EyeVector;
	FRotator EyeRotator;
	GetWorld()-> GetFirstPlayerController()->GetPlayerViewPoint(EyeVector, EyeRotator);
	FVector LineTrace = EyeVector + EyeRotator.Vector() * Reach;

	// DebugLine
	if(DebugLine)
	{
		DrawDebugLine
		(
			GetWorld(),
			EyeVector,
			LineTrace,
			FColor(0,255,0),
			false,
			0.f,
			0,
			5.f
		);
	}

	//Grabbing items
	if (PhysicsHandle == nullptr){return;}
	if(PhysicsHandle->GrabbedComponent)
		PhysicsHandle->SetTargetLocation(LineTrace);
}

