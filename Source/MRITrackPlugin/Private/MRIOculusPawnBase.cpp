// Fill out your copyright notice in the Description page of Project Settings.

#include "MRIOculusPawnBase.h"
#include "MRICaptureVolume.h"

#include "Core.h"
#include "PlatformTime.h"
#include "PlatformFile.h"
#include "PlatformFileManager.h"


// Sets default values
AMRIOculusPawnBase::AMRIOculusPawnBase(const FObjectInitializer & ObjectInitializer)
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    SetTickGroup(TG_PrePhysics);
    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->bLockToHmd = false;
    CameraComponent->SetupAttachment(SceneComponent);
    //CameraComponent->SetRelativeRotation(FRotator(0,90,0));
    trackId = 0;
}

// Called when the game starts or when spawned
void AMRIOculusPawnBase::BeginPlay()
{
    Super::BeginPlay();

    lastRotationCorrection = FQuat::Identity;

    // load sensor fusion dll
    FString dllName = "HmdDriftCorrection.dll";
    FString path = "C:/Projects/ue4prototype/Plugins/mritrackplugin/ThirdParty/MRI/lib/Win64/HmdDriftCorrection.dll";

    dllHandle = FPlatformProcess::GetDllHandle(*path);
    if (dllHandle != nullptr)
    {
        // set up sensor fusion functions
        sf_create = (NpHmd_Create)FPlatformProcess::GetDllExport(dllHandle, TEXT("NpHmd_Create"));
        sf_destroy = (NpHmd_Destroy)FPlatformProcess::GetDllExport(dllHandle, TEXT("NpHmd_Destroy"));
        sf_update = (NpHmd_MeasurementUpdate)FPlatformProcess::GetDllExport(dllHandle, TEXT("NpHmd_MeasurementUpdate"));
        sf_getCorrection = (NpHmd_GetOrientationCorrection)FPlatformProcess::GetDllExport(dllHandle, TEXT("NpHmd_GetOrientationCorrection"));

        // initialize hmd handle
        sf_create(&hmdHandle);
        hasInitializedSensorFusion = true;
    }
    else
    {
        checkf(false, TEXT("Failed to load dll"));
    }
}

void AMRIOculusPawnBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    if (hasInitializedSensorFusion)
    {
        sf_destroy(hmdHandle);
    }
}

// Called every frame
void AMRIOculusPawnBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


}

void AMRIOculusPawnBase::CalculateHmd(const float & delta, const FRotator & inertialRotation)
{
    if (CaptureVolume == nullptr)
    {
        CaptureVolume = AMRICaptureVolume::FindDefaultCaptureVolume(GetWorld());
    }

    FTransform finalTransform(GetActorTransform());

    FQuat inertialQuat = inertialRotation.Quaternion();

    // get mocap data
    FVector opticalPos;
    FQuat opticalRot;
    bool success = CaptureVolume->GetLatestHmdTransform(trackId, opticalPos, opticalRot);

    if (success)
    {
        //// Tracking is y-up, world is z-up. TODO: consider getting this from TrackClient?
        ////auto axisCorrection = FRotator(0, 0, 90).Quaternion();
        //auto axisCorrection = FQuat::MakeFromEuler(FVector(90,0,0));
        //opticalPos = opticalRot.Inverse() * opticalPos;
        //opticalRot = axisCorrection * opticalRot;

        if (hasInitializedSensorFusion)
        {
            auto optical_NpQuat = FQuatToNpQuaternion(opticalRot);
            auto inertial_NpQuat = FQuatToNpQuaternion(inertialQuat);
            NpHmdResult fuseResult = sf_update(hmdHandle, &optical_NpQuat, &inertial_NpQuat, delta);

            if (fuseResult == NpHmdResult::OK)
            {
                NpHmdQuaternion fusedQuat;
                if (sf_getCorrection(hmdHandle, &fusedQuat) == NpHmdResult::OK)
                {
                    lastRotationCorrection = NpQuaternionToFQuat(fusedQuat);
                }
            }
        }

        // set position
        finalTransform.SetLocation(opticalPos);
    }

    // set rotation
    //finalTransform.SetRotation(inertialQuat * lastRotationCorrection);
    finalTransform.SetRotation(opticalRot);


    // apply final transformation
    SetActorTransform(finalTransform);
    //CameraComponent->SetRelativeRotation(FRotator::MakeFromEuler(FVector(0,0,90)));


    // debug
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(*opticalRot.Euler().ToString()));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(*r1.Euler().ToString()));
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(*output.Euler().ToString()));
}

NpHmdQuaternion AMRIOculusPawnBase::FQuatToNpQuaternion(const FQuat & quat)
{
    return NpHmdQuaternion{ -quat.X, quat.Z, quat.Y, quat.W };
}

FQuat AMRIOculusPawnBase::NpQuaternionToFQuat(const NpHmdQuaternion & quat)
{
    return FQuat(-quat.x, quat.z, quat.y, quat.w);
}

NpHmdQuaternion AMRIOculusPawnBase::RotatorToNpQuaternion(const FRotator & rotator)
{
    return FQuatToNpQuaternion(rotator.Quaternion());
}

FRotator AMRIOculusPawnBase::NpQuaternionToRotator(const NpHmdQuaternion & quat)
{
    return FRotator(NpQuaternionToFQuat(quat));
}


