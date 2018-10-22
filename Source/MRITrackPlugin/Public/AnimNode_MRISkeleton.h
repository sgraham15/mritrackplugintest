
#pragma once

#include <Animation/AnimNodeBase.h>
#include <array>
#include <map>

#include "MRISkeletonTypes.h"

#include "AnimNode_MRISkeleton.generated.h"


USTRUCT( BlueprintType )
struct MRITRACKPLUGIN_API FAnimNode_MRISkeleton : public FAnimNode_Base
{
	GENERATED_BODY()

public:
    FAnimNode_MRISkeleton();

	void Initialize_AnyThread( const FAnimationInitializeContext& Context ) override;
	void CacheBones_AnyThread( const FAnimationCacheBonesContext& Context ) override;
	void Update_AnyThread( const FAnimationUpdateContext& Context ) override;
	void Evaluate_AnyThread( FPoseContext& Output ) override;
	//bool CanUpdateInWorkerThread() const override { return false; }

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = ( PinShownByDefault ) )
    FName SourceSkeletonAssetName;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = ( PinShownByDefault ) )
    class AMRICaptureVolume *CaptureVolume;

    // Scale UE bones to match tracked skeleton
    //UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = ( PinShownByDefault ) )
    //bool ScaleBones{ true };

    UPROPERTY( EditAnywhere, EditFixedSize, Category = "Retargeting" )
    TArray<FBoneReference> BoneMappings;

private:
    static const int32 kInvalidBoneStreamingId = -1;
    static const uint8 kMaxSourceBones = static_cast< uint8 >( EMRIBone::NumBones );

    void PerformStreamingRetargeting( FPoseContext& PoseContext, uint8 BoneId );
    bool TryUpdateSkeletonDefinition();
    void MarkSkeletonDefinitionInvalid();
    FName GetMappedBoneName( int BoneId );
    void CacheStreamingBoneIds();

    template< typename PoseT >
    void DrawPose( FAnimInstanceProxy* DrawProxy, FCSPose< PoseT >& CsPose, const FColor& DrawColor );

    bool SourceSkeletonDefinitionValid{ false };
    FMRISkeletonDefinition SourceSkeletonDefinition;
    FMRISkeletonState LatestSkeletonState;
    std::array<int32, kMaxSourceBones> BoneStreamingIds;
    std::map<int32, int32> BoneMap;
};
