// Fill out your copyright notice in the Description page of Project Settings.

#include "MRIOculusPawn.h"
#include "MRICaptureVolume.h"

#include "Core.h"
#include "PlatformTime.h"
#include "PlatformFile.h"
#include "PlatformFileManager.h"

//*
typedef NpHmdResult(* NpHmd_Create)( void ** hmdHandle );
typedef NpHmdResult (* NpHmd_Destroy)( void * hmdHandle );

typedef NpHmdResult (*NpHmd_MeasurementUpdate)( void * hmdHandle,
                                     NpHmdQuaternion * opticalOrientation,
                                     NpHmdQuaternion * inertialOrientation,
                                     float delta );

typedef NpHmdResult (*NpHmd_GetOrientationCorrection)( void * hmdHandle, NpHmdQuaternion * outOrientation );

//*/

// Sets default values
AMRIOculusPawn::AMRIOculusPawn(const FObjectInitializer & ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    SetTickGroup(TG_PrePhysics);
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    trackId = 0;


}

//// Called when the game starts or when spawned
//void AMRIOculusPawn::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

void AMRIOculusPawn::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
    Super::EndPlay( EndPlayReason );
    if (hasInitializedSensorFusion)
    {
        FString destroyFuncName = "NpHmd_Destroy";
        auto destroyFunc = (NpHmd_Destroy)FPlatformProcess::GetDllExport(dllHandle, *destroyFuncName);
        destroyFunc(hmdHandle);
    }
}

// Called every frame
void AMRIOculusPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AMRIOculusPawn::CalculateHmd(const float & delta, const FRotator & inertialRotation, bool & result, FVector & fusedPosition, FRotator & fusedRotation)
{
    result = false;

    if (CaptureVolume == nullptr)
    {
        CaptureVolume = AMRICaptureVolume::FindDefaultCaptureVolume(GetWorld());
    }

    FVector opticalPos;
    FQuat opticalRot;
    bool success = CaptureVolume->GetLatestHmdTransform(trackId, opticalPos, opticalRot);
    if (success)
    {
        if (!hasInitializedSensorFusion)
        {
            FString dllName = "HmdDriftCorrection.dll";
            FString path = "C:/Projects/ue4prototype/Plugins/mritrackplugin/ThirdParty/MRI/lib/Win64/HmdDriftCorrection.dll";

            dllHandle = FPlatformProcess::GetDllHandle(*path);
            if( dllHandle != nullptr )
            //if (SearchForAndLoadDll(dllHandle, FPaths::GamePluginsDir(), dllName))
            {
                FString createFuncName = "NpHmd_Create";
                auto createFunc = (NpHmd_Create)FPlatformProcess::GetDllExport( dllHandle, *createFuncName);
                createFunc(&hmdHandle);
                //NpHmd_Create(&hmdHandle);
                hasInitializedSensorFusion = true;
            }
            else
            {
                checkf(false, TEXT("Failed to load dll"));
            }

            
        }

        if (hasInitializedSensorFusion)
        {
            FString updateFuncName = "NpHmd_MeasurementUpdate";
            FString getFuncName = "NpHmd_GetOrientationCorrection";

            auto updateFunc = (NpHmd_MeasurementUpdate)FPlatformProcess::GetDllExport( dllHandle, *updateFuncName );
            auto getFunc = (NpHmd_GetOrientationCorrection)FPlatformProcess::GetDllExport(dllHandle, *getFuncName);

            auto opticalQuat = FQuatToNpQuaternion(opticalRot);
            auto inertialQuat = RotatorToNpQuaternion(inertialRotation);

            NpHmdResult fuseResult = updateFunc( hmdHandle, &opticalQuat, &inertialQuat, delta );
            if (fuseResult == NpHmdResult::OK)
            {
                NpHmdQuaternion fusedQuat;
                if (getFunc(hmdHandle, &fusedQuat) == NpHmdResult::OK)
                {
                    //fusedPosition = opticalPos;
                    //fusedRotation = NpQuaternionToRotator(fusedQuat);
                    fusedPosition = opticalPos;
                    fusedRotation = FRotator(0,0,0);
                    result = true;
                }
            }
        }
    }
}

NpHmdQuaternion AMRIOculusPawn::FQuatToNpQuaternion(const FQuat & quat )
{
    return NpHmdQuaternion{ quat.X, quat.Y, quat.Z, quat.W };
}

FQuat AMRIOculusPawn::NpQuaternionToFQuat(const NpHmdQuaternion & quat)
{
    return FQuat(quat.x, quat.y, quat.z, quat.w);
}

NpHmdQuaternion AMRIOculusPawn::RotatorToNpQuaternion(const FRotator & rotator)
{
    return FQuatToNpQuaternion(rotator.Quaternion());
}

FRotator AMRIOculusPawn::NpQuaternionToRotator(const NpHmdQuaternion & quat)
{
    return FRotator(NpQuaternionToFQuat(quat));
}

bool AMRIOculusPawn::SearchForAndLoadDll(void * dllHandle, FString _searchBase, FString _dllName)
{
    //Search Plugins folder for an instance of Dll.dll, and add to platform search path
    TArray<FString> directoriesToSkip;
    IPlatformFile &PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
    FLocalTimestampDirectoryVisitor Visitor(PlatformFile, directoriesToSkip, directoriesToSkip, false);
    PlatformFile.IterateDirectory(*_searchBase, Visitor);

    for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
    {
        const FString file = TimestampIt.Key();
        const FString filePath = FPaths::GetPath(file);
        const FString fileName = FPaths::GetCleanFilename(file);
        if (fileName.Compare(_dllName) == 0)
        {
            FPlatformProcess::AddDllDirectory(*filePath); // only load dll when needed for use. Broken with 4.11.
            dllHandle = FPlatformProcess::GetDllHandle(*file); // auto-load dll with plugin - needed as 4.11 breaks above line.
            return true;
        }
    }
    return false;
}

//// Called to bind functionality to input
//void AMRIOculusPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

