// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <Camera/CameraComponent.h>
#include "HmdDriftCorrection.h"

#include "MRIOculusPawnBase.generated.h"


// sensor fusion functions
typedef NpHmdResult(*NpHmd_Create)(void ** hmdHandle);
typedef NpHmdResult(*NpHmd_Destroy)(void * hmdHandle);

typedef NpHmdResult(*NpHmd_MeasurementUpdate)(void * hmdHandle,
    NpHmdQuaternion * opticalOrientation,
    NpHmdQuaternion * inertialOrientation,
    float delta);

typedef NpHmdResult(*NpHmd_GetOrientationCorrection)(void * hmdHandle, NpHmdQuaternion * outOrientation);


UCLASS()
class MRITRACKPLUGIN_API AMRIOculusPawnBase : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    AMRIOculusPawnBase(const FObjectInitializer & ObjectInitializer);

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "MRI")
    class AMRICaptureVolume * CaptureVolume;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRI")
    int trackId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent * SceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent * CameraComponent;

private:
    bool hasInitializedSensorFusion{ false };
    void * hmdHandle;
    void * dllHandle;

    FQuat lastRotationCorrection;

    NpHmd_Create sf_create;
    NpHmd_Destroy sf_destroy;
    NpHmd_MeasurementUpdate sf_update;
    NpHmd_GetOrientationCorrection sf_getCorrection;

    static NpHmdQuaternion FQuatToNpQuaternion(const FQuat & quat);
    static FQuat NpQuaternionToFQuat(const NpHmdQuaternion & quat);
    static NpHmdQuaternion RotatorToNpQuaternion(const FRotator & rotator);
    static FRotator NpQuaternionToRotator(const NpHmdQuaternion & quat);

public:
    UFUNCTION(BlueprintCallable, Category = "MRI")
    void CalculateHmd(const float & delta, const FRotator & inertialRotation);

};
