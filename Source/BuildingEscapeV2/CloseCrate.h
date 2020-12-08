// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/TriggerVolume.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CloseCrate.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPEV2_API UCloseCrate : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCloseCrate();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OpenCrate(float DeltaTime);
	void CloseCrate(float DeltaTime);
	float TotalMassOfActors() const;
	void FindPressurePlate();

private:
	float TargetZ;
	float CurrentZ;
	float CloseZ;

	UPROPERTY(EditAnywhere)
	float OpenHight = -90.f;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate = nullptr;

	AActor* ActorThatOpen = nullptr;

	UPROPERTY(EditAnywhere)
	bool OpenViaMass = 0;

	UPROPERTY(EditAnywhere)
	float MassToOpen = 0;

	float CrateLastOpened;

	UPROPERTY(EditAnywhere)
	float CrateCloseDelay = 0.f;

	UPROPERTY(EditAnywhere)
	float CrateOpenSpeed = 0.5f;

	UPROPERTY(EditAnywhere)
	float CrateCloseSpeed = 2.f;

		
};
