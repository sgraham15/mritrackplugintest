#pragma once

#include <Components/ActorComponent.h>
#include "MRICaptureVolume.h"
#include "MRISkeletonTypes.h"
#include "MRISkeletonComponent.generated.h"



/// A component that drives the position and rotation of its actor's root
/// component based on the extracted XY planar movement of a tracked skeleton's
/// root motion / hip movement. Designed to work with FAnimNode_OptitrackSkeleton.
UCLASS( ClassGroup = "MRI", meta = ( BlueprintSpawnableComponent ) )
class MRITRACKPLUGIN_API UMRISkeletonComponent : public UActorComponent
{
    GENERATED_BODY()

    UMRISkeletonComponent( const FObjectInitializer& ObjectInitializer );

public:
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "MRI" )
    FName SourceSkeletonAssetName;

    // If your scene contains multiple client origin objects, you can specify
    // which one to use. If unset, this defaults to the first client origin
    // that's found in the world.
    UPROPERTY( EditAnywhere, AdvancedDisplay, BlueprintReadWrite, Category = "MRI" )
    class AMRICaptureVolume* MRICaptureVolume;

protected:
    void BeginPlay() override;
    void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

    UPROPERTY()
    class UMRIAnimInstance* MRIAnimInstance;

    FMRISkeletonDefinition SkeletonDefinition;
};
