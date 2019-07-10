
#pragma once

#include "MRISkeletonTypes.generated.h"

// Motive is +Z forward, which after the UE4 change of basis/handedness is +Y forward.
// We want +X forward, so this applies that 90 degree rotation about Z.
extern const FQuat kMRISkeletonQuatBasisChange;
extern const FQuat kMRISkeletonQuatBasisChangeInv;

USTRUCT( BlueprintType )
struct FMRIPose
{
    GENERATED_BODY()

        UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        FVector Position;

    UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        FQuat Orientation;
};

UENUM( BlueprintType )
enum class EMRIBoneNamingConvention : uint8
{
    Motive,
    FBX,
    BVH,
};

UENUM( BlueprintType )
enum class EMRIBone : uint8
{
    Root = 0,
    Hip,
    Ab,
    Chest,
    Neck,
    Head,

    LShoulder,
    LUArm,
    LFArm,
    LHand,

    RShoulder,
    RUArm,
    RFArm,
    RHand,

    LThigh,
    LShin,
    LFoot,
    LToe,

    RThigh,
    RShin,
    RFoot,
    RToe,

    LThumb1, LThumb2, LThumb3,
    LIndex1, LIndex2, LIndex3,
    LMiddle1, LMiddle2, LMiddle3,
    LRing1, LRing2, LRing3,
    LPinky1, LPinky2, LPinky3,

    RThumb1, RThumb2, RThumb3,
    RIndex1, RIndex2, RIndex3,
    RMiddle1, RMiddle2, RMiddle3,
    RRing1, RRing2, RRing3,
    RPinky1, RPinky2, RPinky3,

    NumBones,
};


FText MRITRACKPLUGIN_API GetDisplayBoneName( EMRIBone BoneId );
FName MRITRACKPLUGIN_API GetStreamingBoneName( EMRIBone BoneId, EMRIBoneNamingConvention NamingConvention );
EMRIBone MRITRACKPLUGIN_API GetParentBone( EMRIBone BoneId );


// The streaming skeleton begins at the hip bone; the root bone is implicit, and not streamed.
USTRUCT( BlueprintType )
struct FMRIBoneDefinition
{
    GENERATED_BODY()

        UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        int Id;

    UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        int ParentId;

    UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        FName Name;

    UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        FVector LocalOffset;
};


USTRUCT( BlueprintType )
struct FMRISkeletonDefinition
{
    GENERATED_BODY()

        UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        int Id;

    UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        FName Name;

    UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        TArray< FMRIBoneDefinition > Bones;
};


USTRUCT( BlueprintType )
struct FMRISkeletonState
{
    GENERATED_BODY()

        UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        TArray< FMRIPose > BonePoses;

    UPROPERTY( BlueprintReadOnly, Category = "MRI" )
        TArray< FTransform > WorldSpaceBoneTransforms;
};
