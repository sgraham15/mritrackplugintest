#include "AnimNode_MRISkeleton.h"
#include "MRICaptureVolume.h"

#include <Animation/AnimInstanceProxy.h>
#include <DrawDebugHelpers.h>
#include "TrackNetClientCAPI.h"

//TArray<FName> FAnimNode_MRISkeleton::BoneNames;
//const int32 FAnimNode_MRISkeleton::kInvalidBoneStreamingId;// = -1;
///const uint8 FAnimNode_MRISkeleton::kMaxSourceBones;// = static_cast< uint8 >( EMRIBone::NumBones );

FAnimNode_MRISkeleton::FAnimNode_MRISkeleton()
{
	const uint8 kNumBones = static_cast<uint8>(EMRIBone::NumBones);
	BoneMappings.SetNum( kNumBones );
	MarkSkeletonDefinitionInvalid();


    /*
    BoneMap = {
        { 0, 0 }, // root
        { 1, 1 }, // pelvis
        { 3, 2 }, // spine_01
        { 4, 3 }, // spine_02
        
        { 5, 3 }, // spine_03
        { 8, 3 }, // clavicle_l

    };
    */

}


void FAnimNode_MRISkeleton::Initialize_AnyThread( const FAnimationInitializeContext& Context )
{
	Super::Initialize_AnyThread( Context );

	MarkSkeletonDefinitionInvalid();
}

void FAnimNode_MRISkeleton::CacheBones_AnyThread( const FAnimationCacheBonesContext& Context )
{
    /*
	const FBoneContainer& RequiredBones = Context.AnimInstanceProxy->GetRequiredBones();

	for ( FBoneReference& BoneRef : BoneMappings )
	{
        BoneRef.Initialize( RequiredBones );
	}

    FCompactPose RefPose;
	RefPose.SetBoneContainer( &RequiredBones );
	RefPose.ResetToRefPose();

    // compute and cache reference world positions
    ComputePoseCorrectionTransforms(RefPose);

	FCSPose<FCompactPose> CsRefPose;
	CsRefPose.InitPose( RefPose );
    
	// Composite measurements
	const FVector HipPos = GetCsBoneLocation( CsRefPose, EOptitrackBone::Hip );
	const FVector HeadPos = GetCsBoneLocation( CsRefPose, EOptitrackBone::Head );
	const FVector LFootPos = GetCsBoneLocation( CsRefPose, EOptitrackBone::LFoot );
	const FVector RFootPos = GetCsBoneLocation( CsRefPose, EOptitrackBone::RFoot );
	TargetScaling.TorsoLength = (HeadPos - HipPos).Size();
	TargetScaling.AvgLegLength = ((LFootPos - HipPos).Size() + (RFootPos - HipPos).Size()) / 2.0f;

	const FVector LHandPos = GetCsBoneLocation( CsRefPose, EOptitrackBone::LHand );
	const FVector RHandPos = GetCsBoneLocation( CsRefPose, EOptitrackBone::RHand );
	const FVector LShoulderPos = GetCsBoneLocation( CsRefPose, EOptitrackBone::LShoulder );
	const FVector RShoulderPos = GetCsBoneLocation( CsRefPose, EOptitrackBone::RShoulder );
	const float IntraShoulderDist = (LShoulderPos - RShoulderPos).Size();
	const float IntraHandDist = (LHandPos - RHandPos).Size();
	TargetScaling.AvgArmLength = (IntraHandDist - IntraShoulderDist) / 2.0f;

    // initialize ignore bone array
    const TArray<int32> PoseIndexArray = RequiredBones.GetSkeletonToPoseBoneIndexArray();
    IgnoreBoneIndices.Reset(kMaxSourceBones);
    for (int32 BoneIndex : PoseIndexArray)
    {
        bool BoneIndexInBoneMappings = false;
        for (FBoneReference& BoneRef : BoneMappings)
        {
            if (BoneRef.BoneIndex == BoneIndex)
            {
                BoneIndexInBoneMappings = true;
                break;
            }
        }
        if (!BoneIndexInBoneMappings)
        {
            // add to ignore bone list
            IgnoreBoneIndices.Push(BoneIndex);
        }
    }

    // Exact segment lengths - skip the root bone.
    TargetScaling.BoneDistanceToParent[0] = 0.0f;
    for (uint8 BoneId = 1; BoneId < static_cast<uint8>(EOptitrackBone::NumBones); ++BoneId)
    {
        EOptitrackBone Bone = static_cast<EOptitrackBone>(BoneId);
        float BoneLength = GetCsBoneDistanceToParent(CsRefPose, Bone);
        if (BoneLength == 0.0f)
        {
            TargetScaling.BoneDistanceToParent[BoneId] = 1.0f;
        }
        else
        {
            TargetScaling.BoneDistanceToParent[BoneId] = BoneLength;
        }
    }
    */
}

void FAnimNode_MRISkeleton::MarkSkeletonDefinitionInvalid()
{
    SourceSkeletonDefinitionValid = false;

    for( int32& BoneStreamingId : BoneStreamingIds )
    {
        BoneStreamingId = kInvalidBoneStreamingId;
    }

    //ResetTargetScaleRatios();
}



void FAnimNode_MRISkeleton::Update_AnyThread( const FAnimationUpdateContext& Context )
{
    //*
	EvaluateGraphExposedInputs.Execute( Context );

	if ( CaptureVolume == nullptr )
	{
		return;
	}

	if ( SourceSkeletonDefinitionValid == false )
	{
		if ( ! TryUpdateSkeletonDefinition() )
		{
			return;
		}
	}

	const bool DataAvailable = CaptureVolume->GetLatestSkeletonState( SourceSkeletonDefinition.Id, LatestSkeletonState );
	if ( DataAvailable == false )
	{
		MarkSkeletonDefinitionInvalid();
		return;
	}

	//ExtractSourceScalingFromLatestState();
	//UpdateTargetScaleRatios();

#if 0
	// HACK?
	const float SourceHeadToAnkle = SourceScaling.TorsoLength + SourceScaling.AvgLegLength;
	const float TargetHeadToAnkle = TargetScaling.TorsoLength + TargetScaling.AvgLegLength;
	const float TargetRootScaleRatio = SourceHeadToAnkle / TargetHeadToAnkle;
	LatestSkeletonState.BonePoses[1].Position.Z /= (TargetRootScaleRatio * TargetRootScaleRatio);
#endif
//*/
}




FName FAnimNode_MRISkeleton::GetMappedBoneName( int BoneId )
{
    return "";
}


void FAnimNode_MRISkeleton::CacheStreamingBoneIds()
{
    //for( int BoneIdx = 0; BoneIdx < kMaxSourceBones; ++BoneIdx )
    //{
    //    BoneStreamingIds[BoneIdx] = FindBoneId( GetMappedBoneName( BoneIdx ) );
    //}
}


bool FAnimNode_MRISkeleton::TryUpdateSkeletonDefinition()
{
    if( CaptureVolume->FindSkeletonDefinition( SourceSkeletonAssetName, SourceSkeletonDefinition ) )
    {
        SourceSkeletonDefinitionValid = true;

        CacheStreamingBoneIds();// CaptureVolume > BoneNamingConvention );

        // Compute segment length adjustments as anisotropic bone scale factors.
        //ExtractSourceScalingFromDefinition();
        //UpdateTargetScaleRatios();

        return true;
    }
    
    return false;
}


void FAnimNode_MRISkeleton::Evaluate_AnyThread( FPoseContext& Output )
{
    if( ( CaptureVolume == nullptr ) || !CaptureVolume->HasCreatedTrackClientFunc() )
    {
        return;
    }

    //auto regCount = CAPI_GetRegisteredEntityCount();
    //if( regCount == 0 )
    //{
    //    return;
    //}

    if( BoneMap.empty() && CaptureVolume && CaptureVolume->HasCreatedTrackClientFunc() )
    {
        //int numEntities = CAPI_GetNumLocalEntities();
        {
            auto numBones = Output.Pose.GetBoneContainer().GetReferenceSkeleton().GetNum();
            for( int ue4Idx = 0; ue4Idx < numBones; ++ue4Idx )
            {
                auto boneName = Output.Pose.GetBoneContainer().GetReferenceSkeleton().GetBoneName( ue4Idx ).ToString();
                const char *ansiBoneName = TCHAR_TO_ANSI( *boneName );
                int mriIdx = CAPI_GetBoneIndexByName( 0, ansiBoneName );
                if( mriIdx >= 0 )
                {
                    BoneMap.insert( std::make_pair( mriIdx, ue4Idx ) );
                }
            }
        }
    }

	Output.Pose.ResetToRefPose();
    //auto boneCount = 35;
    static float x = 0.0f;
    int32_t boneCount = 0;
    QuatVec3f *poses = nullptr;
    //CAPI_GetAllBonePosesAbsolute( 0, &poses, &boneCount );
    CAPI_GetAllBonePosesRelativeWithOverride( 0, &poses, &boneCount, ConversionDescriptorMode::UNREALENGINE_RELATIVE );

    if( poses != nullptr )
    {
        for( auto const &boneMapPair : BoneMap )
        {
            int mriIdx = boneMapPair.first;
            int ue4Idx = boneMapPair.second;

            const QuatVec3f &quatVec = poses[mriIdx];
            FCompactPoseBoneIndex fcpIdx{ ue4Idx };
   
            Output.Pose[fcpIdx].SetRotation( FQuat( quatVec.q.x, quatVec.q.y, quatVec.q.z, quatVec.q.w ) );
            Output.Pose[fcpIdx].SetTranslation( FVector( quatVec.v.x, quatVec.v.y, quatVec.v.z ) );
            if (ue4Idx == 0)
            {
	            //// Tracking is y-up, world is z-up. TODO: consider getting this from TrackClient?
	            //FQuat rootRot(quatVec.q.x, quatVec.q.y, quatVec.q.z, quatVec.q.w);
	            //auto xAxisRot = FRotator(0, 0, 90).Quaternion();
	            //xAxisRot *= rootRot;
	            //Output.Pose[fcpIdx].SetRotation( xAxisRot);
	            //Output.Pose[fcpIdx].SetTranslation(rootRot.Inverse() * Output.Pose[fcpIdx].GetTranslation());

                // still need to rotate characters 90 degrees
                const auto rootQuatVec = CAPI_GetBonePoseAbsoluteWithOverride(0, 0, ConversionDescriptorMode::UNREALENGINE );
                FVector rootPos(FVector(rootQuatVec.v.x, rootQuatVec.v.y, rootQuatVec.v.z));
                FQuat rootRot(rootQuatVec.q.x, rootQuatVec.q.y, rootQuatVec.q.z, rootQuatVec.q.w);
                rootRot *= FQuat::MakeFromEuler(FVector(0,0,-90)); // is this right?

                Output.Pose[fcpIdx].SetRotation(rootRot);
                Output.Pose[fcpIdx].SetTranslation(rootPos);
            }
		    
            UE_LOG( LogTemp, Warning, TEXT( "Pos: x: %f, y: %f, z: %f" ), quatVec.v.x, quatVec.v.y, quatVec.v.z );
            Output.Pose[fcpIdx].DiagnosticCheck_IsValid();
        }

        // Debug Draw
        {
            FCSPose<FCompactPose> CsPose;
            CsPose.InitPose( Output.Pose );
            DrawPose( Output.AnimInstanceProxy, CsPose, FColor::Orange );
        }
    }

    



    /*
    auto boneCount = static_cast< int >( EMRIBone::NumBones );
    auto numBones = Output.Pose.GetNumBones();
    for( int i = 0; i < 50; ++i )  // this is a made up value
    {
        if( i == 12 || i == 40 )
        {
            FCompactPoseBoneIndex fcpIdx{ i };
            auto t = Output.Pose[fcpIdx].GetTranslation();

            static float angle = 0;

            angle += .02;

            if( angle > PI * 2 )
            {
                angle = 0;
            }


            FQuat quat( sinf( angle / 2 ), 0, 0, cosf( angle / 2 ) );
            x += 0.05;
            FVector eulers( x, 0, 0 );
            Output.Pose[fcpIdx].SetRotation( quat * Output.Pose[fcpIdx].GetRotation() * quat.Inverse() );
            //x += 0.05;
            //t.X = x;

            Output.Pose[fcpIdx].SetTranslation( t );
            //Output.Pose[fcpIdx].SetTranslation( refPose.GetTranslation() + retargetTranslation );
            Output.Pose[fcpIdx].DiagnosticCheck_IsValid();
        }
    }
    */


    //for( int BoneId = 0; BoneId < boneCount; ++BoneId )
    //{
    //    PerformStreamingRetargeting( Output, BoneId );
    //}

    //for( int i = 0; i < boneCount; ++i )
    //{
    //    Output.Pose[i].SetRotation( PoseRotation );
    //    Output.Pose[i].SetTranslation( ParentToChildTransform.GetTranslation() );
    //}
    /*
    Output.Pose.ResetToRefPose();
#if 0
	if ( RetargetBaseMesh != Output.AnimInstanceProxy->GetSkelMeshComponent()->SkeletalMesh )
	{
		return;
	}
#endif

	for ( int BoneId = 0; BoneId < static_cast<int>(EOptitrackBone::NumBones); ++BoneId )
	{
		PerformStreamingRetargeting( Output, BoneId );
	}

	for ( const FCompactPoseBoneIndex fcpIdx : Output.Pose.ForEachBoneIndex() )
	{
		Output.Pose[fcpIdx].DiagnosticCheck_IsValid();
	}

//#if OPTITRACK_SKELNODE_DEBUGDRAW
    if (StreamingClientOrigin && StreamingClientOrigin->bDrawDebugSkeletons)
	{
		FCSPose<FCompactPose> CsPose;
		CsPose.InitPose( Output.Pose );
		DrawPose( Output.AnimInstanceProxy, CsPose, FColor::Green );
	}
//#endif
*/
}

void FAnimNode_MRISkeleton::PerformStreamingRetargeting( FPoseContext& PoseContext, uint8 BoneId )
{
    //if( BoneId >= BoneMappings.Num() || BoneId >= kMaxSourceBones )
    //{
    //    return;
    //}

    auto numBones = PoseContext.Pose.GetNumBones();
    int i = 0;//for( int i = 0; i < numBones; ++i )
    {
        FCompactPoseBoneIndex fcpIdx{ i };
        auto t = PoseContext.Pose[fcpIdx].GetTranslation();
        t.X += 0.05;
        PoseContext.Pose[fcpIdx].SetTranslation( t );
        //PoseContext.Pose[fcpIdx].SetTranslation( refPose.GetTranslation() + retargetTranslation );
        PoseContext.Pose[fcpIdx].DiagnosticCheck_IsValid();
    }

    /*
    FBoneReference& BoneRef = BoneMappings[BoneId];
    const int32 BoneStreamingId = BoneStreamingIds[BoneId];

    if( BoneStreamingId < 0 || BoneStreamingId >= LatestSkeletonState.BonePoses.Num() )
    {
        return;
    }

    const FBoneContainer& RequiredBones = PoseContext.Pose.GetBoneContainer();
    if( BoneRef.IsValidToEvaluate( RequiredBones ) )
    {
        const FCompactPoseBoneIndex fcpIdx = BoneRef.GetCompactPoseIndex( RequiredBones );
        LatestSkeletonState.BonePoses

        const FMRIPose& streamingPose = LatestSkeletonState.BonePoses[BoneStreamingId];
        const FTransform refPose = PoseContext.Pose[fcpIdx];

        PoseContext.Pose[fcpIdx].SetRotation(
            refPose.GetRotation()
            // * kMRISkeletonQuatBasisChangeInv
            * streamingPose.Orientation
            // * kMRISkeletonQuatBasisChange
            * refPose.GetRotation().Inverse()
        );

        auto b = PoseContext.Pose.GetBones();
        PoseContext.Pose[fcpIdx].ToHumanReadableString();
        const FVector streamingTranslation = kMRISkeletonQuatBasisChangeInv.RotateVector( streamingPose.Position );
        const FVector neutralOffset = SourceSkeletonDefinition.Bones[BoneStreamingId].LocalOffset;
        const FVector retargetTranslation = streamingPose.Position - neutralOffset;
        PoseContext.Pose[fcpIdx].SetTranslation( refPose.GetTranslation() + retargetTranslation );

        PoseContext.Pose[fcpIdx].DiagnosticCheck_IsValid();
    }
    */
}

//#if OPTITRACK_SKELNODE_DEBUGDRAW
template< typename PoseT >
void FAnimNode_MRISkeleton::DrawPose( FAnimInstanceProxy* DrawProxy, FCSPose< PoseT >& CsPose, const FColor& DrawColor )
{
    if( DrawProxy == nullptr )
    {
        return;
    }

    const FTransform ComponentToWorld = DrawProxy->GetSkelMeshComponent()
        ? DrawProxy->GetSkelMeshComponent()->GetComponentToWorld()
        : FTransform::Identity;

    for( const FCompactPoseBoneIndex fcpIdx : CsPose.GetPose().ForEachBoneIndex() )
    {
        FTransform BoneTM = CsPose.GetComponentSpaceTransform( fcpIdx );
        BoneTM.NormalizeRotation();

        FVector Start, End;
        FColor LineColor = DrawColor;

        End = BoneTM.GetLocation();

        const FCompactPoseBoneIndex fcpParentIdx = CsPose.GetPose().GetParentBoneIndex( fcpIdx );
        if( fcpParentIdx >= 0 )
        {
            FTransform ParentTM = CsPose.GetComponentSpaceTransform( fcpParentIdx );
            ParentTM.NormalizeRotation();
            Start = ParentTM.GetLocation();
        }
        else
        {
            Start = FVector::ZeroVector;//ComponentToWorld.GetLocation();
            LineColor = FColor::Red;
        }

        static const float SphereRadius = 1.0f;

        // Calc cone size
#if 0
        //auto World = GetComponentToWorld();
        //FTransform World = FTransform::Identity;
        //auto wCtx = GEngine->GetWorldContexts();
        //auto World = wCtx.World();
        auto *World = CaptureVolume->GetWorld();//GWorld;//->GetWorld();
        //auto World = UWorld::GetWorld();
        FVector EndToStart = ( Start - End );
        float ConeLength = EndToStart.Size();
        float Angle = FMath::RadiansToDegrees( FMath::Atan( SphereRadius / ConeLength ) ) * 0.02f;
        DrawDebugCone( World, End, EndToStart, ConeLength, Angle, Angle, 4, LineColor );
        //DrawDebugSphere( World, End, SphereRadius, 10, LineColor );
        DrawDebugCoordinateSystem( World, End, BoneTM.Rotator(), SphereRadius );

        const FMeshPoseBoneIndex MeshIdx =
            CsPose.GetPose().GetBoneContainer().MakeMeshPoseIndex( fcpIdx );
        auto refSkel = CsPose.GetPose().GetBoneContainer().GetReferenceSkeleton();
        const FName& BoneName =
            refSkel.GetRefBoneInfo()[MeshIdx.GetInt()].Name;

        //DrawDebugString( World, End, BoneName.ToString(), nullptr, FColor::White, 0.0f );
#else
        const FVector offset( 100.0f, 0.0f, 0.0f );
        const FVector WorldStart = ComponentToWorld.TransformPosition( Start ) + offset;
        const FVector WorldEnd = ComponentToWorld.TransformPosition( End ) + offset;
        DrawProxy->AnimDrawDebugLine( WorldStart, WorldEnd, LineColor );
        DrawProxy->AnimDrawDebugSphere( WorldEnd, SphereRadius, 10, LineColor );
        //DrawProxy->AnimDrawDebugCoordinateSystem(WorldEnd, BoneTM.Rotator(), SphereRadius * 4.0f, false, -1.f, 0.15f);


        FRotationMatrix R( BoneTM.Rotator() );
        FVector const x = R.GetScaledAxis( EAxis::X );
        FVector const y = R.GetScaledAxis( EAxis::Y );
        FVector const z = R.GetScaledAxis( EAxis::Z );

        auto scale = 10.0f;
        DrawProxy->AnimDrawDebugLine( WorldStart, WorldStart + x * scale, FColor::Red );
        DrawProxy->AnimDrawDebugLine( WorldStart, WorldStart + y * scale, FColor::Green );
        DrawProxy->AnimDrawDebugLine( WorldStart, WorldStart + z * scale, FColor::Blue );

#endif
    }
}
