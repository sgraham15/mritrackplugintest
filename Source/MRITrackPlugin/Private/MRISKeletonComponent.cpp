
#include "MRISkeletonComponent.h"

#include "MRIAnimInstance.h"
#include "MRICaptureVolume.h"
#include "MessageLog.h"


UMRISkeletonComponent::UMRISkeletonComponent( const FObjectInitializer& ObjectInitializer )
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickGroup = TG_LastDemotable;
}


void UMRISkeletonComponent::BeginPlay()
{
    Super::BeginPlay();

    if( MRICaptureVolume == nullptr )
    {
        MRICaptureVolume = AMRICaptureVolume::FindDefaultCaptureVolume( GetWorld() );
        if( MRICaptureVolume == nullptr )
        {
            UE_LOG(LogTemp, Warning, TEXT("UMRISkeletonComponent::BeginPlay(): Unable to locate any capture volumes. Ensure there is one in the scene."));
           // FMessageLog( "PIE" ).Error()
           //     ->AddToken( FUObjectToken::Create( this ) )
           //     ->AddToken( FTextToken::Create( LOCTEXT( "NoCaptureVolume",
           //         "Unable to find capture volume in the scene."
           //     ) ) );
            return;
        }
    }

    // TODO: Set anim node variables
    TInlineComponentArray< USkeletalMeshComponent* > SkelMeshComponents;
    GetOwner()->GetComponents( SkelMeshComponents );
    for( USkeletalMeshComponent* SkelMeshComp : SkelMeshComponents )
    {
        if( UMRIAnimInstance* AnimInstance = Cast<UMRIAnimInstance>( SkelMeshComp->GetAnimInstance() ) )
        {
            MRIAnimInstance = AnimInstance;

#if 0
            MRIAnimInstance->SetRootMotionMode( ERootMotionMode::RootMotionFromEverything );
#endif

            break;
        }
    }

    if( MRIAnimInstance )
    {
        if( MRIAnimInstance->SourceSkeletonAssetName == NAME_None )
        {
            MRIAnimInstance->SourceSkeletonAssetName = SourceSkeletonAssetName;
        }

        if( MRIAnimInstance->CaptureVolume == nullptr )
        {
            MRIAnimInstance->CaptureVolume = MRICaptureVolume;
        }

    }

    /*
    if( !MRICaptureVolume->FindSkeletonDefinition( SourceSkeletonAssetName, SkeletonDefinition ) )
    {
        PrimaryComponentTick.SetTickFunctionEnable( false );
    }
    */
}


void UMRISkeletonComponent::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

#if 0
    if( MRIAnimInstance == nullptr || MRIAnimInstance->GetSkelMeshComponent() == nullptr )
    {
        return;
    }

    USkeletalMeshComponent* SkelMeshComp = MRIAnimInstance->GetSkelMeshComponent();

    FRootMotionMovementParams RootMotion = SkelMeshComp->ConsumeRootMotion();
    RootMotion.Set( SkelMeshComp->ConvertLocalRootMotionToWorld( RootMotion.GetRootMotionTransform() ) );
#endif

    if( MRICaptureVolume == nullptr )
    {
        return;
    }

    /*
    FMRISkeletonState SkelState;
    if( MRICaptureVolume->GetLatestSkeletonState( SkeletonDefinition.Id, SkelState ) )
    {
        const FVector RootPosition = MRICaptureVolume->GetTransform().TransformPosition( SkelState.BonePoses[0].Position );
        //GetOwner()->SetActorLocation( RootPosition );

        const FQuat RootOrientation = MRICaptureVolume->GetTransform().GetRotation() * kMRISkeletonQuatBasisChange * SkelState.BonePoses[0].Orientation;
        //GetOwner()->SetActorRotation( RootOrientation );
    }
    */
}
