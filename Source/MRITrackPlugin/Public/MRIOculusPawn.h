// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <Camera/CameraComponent.h>
#include "HmdDriftCorrection.h"

#include "MRIOculusPawn.generated.h"



UCLASS()
class MRITRACKPLUGIN_API AMRIOculusPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMRIOculusPawn(const FObjectInitializer & ObjectInitializer);

//protected:
	//// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "MRI")
    class AMRICaptureVolume * CaptureVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRI")
    int trackId;
    
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent * CameraComponent;

    bool hasInitializedSensorFusion{ false };
    void * hmdHandle;

    void * dllHandle;

    static NpHmdQuaternion FQuatToNpQuaternion( const FQuat & quat );
    static FQuat NpQuaternionToFQuat( const NpHmdQuaternion & quat );
    static NpHmdQuaternion RotatorToNpQuaternion(const FRotator & rotator);
    static FRotator NpQuaternionToRotator(const NpHmdQuaternion & quat);


    bool SearchForAndLoadDll(void * dllHandle, FString _searchBase, FString _dllName);

    FQuat previousFusionResult;

public:
    UFUNCTION(BlueprintCallable, Category="MRI")
    void CalculateHmd(const float & delta, const FRotator & inertialRotation, bool & result, FVector & fusedPosition, FRotator & fusedRotation);

};
