
#include "MRICaptureVolume.h"

#include <EngineUtils.h>
#include <string>
#include <DrawDebugHelpers.h>
#include "TrackNetClientCAPI.h"


AMRICaptureVolume::AMRICaptureVolume( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
    ServerAddress = "127.0.0.1";
	ClientAddress = "127.0.0.1";
    PrimaryActorTick.bCanEverTick = true;
    /*

    ConnectionType = EOptitrackClientConnectionType::Multicast;
    
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>( this, TEXT( "RootSceneComponent" ) );

#if WITH_EDITORONLY_DATA
	RootComponent->bVisualizeComponent = true;
#endif
*/
}


//void AMRICaptureVolume::PreInitializeComponents()
//{
//	// TODO: Do we need to support this changing at runtime?
//	CachedWorldToMeters = GetWorld()->GetWorldSettings()->WorldToMeters;
//
//	Super::PreInitializeComponents();
//}

void AMRICaptureVolume::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG( LogTemp, Warning, TEXT( "BeginPlay" ) );
    if (bAutoInitialize)
    {
        GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, FString::Printf( TEXT( "YOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO" ) ) );
        //InitializeClient();
        // Do we need path here???  Not used!!!
        HasCreatedTrackClient = CAPI_CreateTrackClient( "Binaries/ThirdParty/MRITrackPluginLibrary/Win64/TrackNetClient.dll", ITRACK_VERSION_NUM, UNREALENGINE );
        auto serverAddrCStr = TCHAR_TO_ANSI( *ServerAddress );
        auto b = CAPI_SetRemote( TCHAR_TO_ANSI( *FString( "http://" + ServerAddress + ":8080" ) ), serverAddrCStr, serverAddrCStr );
        ToggleTracking();
    }
}

void AMRICaptureVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    //if (Client)
    //{
    //    ShutdownClient();
    //}
    
    auto res = CAPI_Shutdown();

}

void AMRICaptureVolume::Tick(float DeltaSeconds)
{
	Super::Tick( DeltaSeconds );

    UE_LOG( LogTemp, Warning, TEXT( "Tick" ) );

	FrameUpdateLock.Lock();

    static int renderFrameNum = 0;

    if( CAPI_ApplyLatestServerData( renderFrameNum++ ) )
    {
        static int instanceId = 0;

        int size = CAPI_GetRosterSize();
        if( size > 0 )
        {
            int count = 0;
            int *toids = nullptr;
            TrackEntityIdWrapper *teids = nullptr;
            CAPI_GetUninitializedTrackEntities( &count, &toids, &teids );

            for( int i = 0; i < count; ++i )
            {
                if( CAPI_SetTrackEntityById( instanceId, toids[i], teids[i] ) )
                {
                    InstanceIds.Add( instanceId );
                    ++instanceId;
                    // TODO: Instantiate prefab!!!
                }
            }
        }

        
#if 0
        for( int id : InstanceIds )
        {
            /*{
                auto bc = CAPI_GetBoneCount( id );
                for( int i = 0; i < bc; ++i )
                {
                    FString name( CAPI_GetBoneNameByIndex( id, i ) );
                    UE_LOG( LogTemp, Warning, TEXT( "%d) %s" ), i, *name );
                }
            }*/

            int32_t boneCount = 0; // CAPI_GetBoneCount( int instanceId );
            QuatVec3f *poses = nullptr;
            CAPI_GetAllBonePosesAbsolute( id, &poses, &boneCount );
            if( boneCount > 0 )
            {
                auto pos = poses[0].v;
                UE_LOG( LogTemp, Warning, TEXT( "Pos: x: %f, y: %f, z: %f" ), pos.x, pos.y, pos.z );
                GEngine->AddOnScreenDebugMessage( -1, 5.f, FColor::Red, FString::Printf( TEXT( "Pos: x: %f, y: %f, z: %f" ), pos.x, pos.y, pos.z ) );
            }
        }
#endif
        //UE_LOG( LogTemp, Warning, TEXT( "Your message" ) );
        
    }

    /*
	if ( bDrawDebugSkeletons )
	{
		for ( const TPair< int32, FOptitrackSkeletonDefinition >& skelDefEntry : SkeletonDefinitions )
		{
			const int32 skelId = skelDefEntry.Key;
			const FOptitrackSkeletonDefinition& skelDef = skelDefEntry.Value;

			if ( ! LatestSkeletonStates.Contains( skelId ) )
			{
				continue;
			}

			const FOptitrackSkeletonState& skelState = LatestSkeletonStates[skelId];

			for ( int32 boneId = 0; boneId < skelDef.Bones.Num(); ++boneId )
			{
				const FOptitrackBoneDefinition& boneDef = skelDef.Bones[boneId];
				const int32 parentBoneId = boneDef.ParentId;
				const FTransform& boneTransform = skelState.WorldSpaceBoneTransforms[boneId];

				FVector Start, End;
				FColor LineColor;

				End = boneTransform.GetLocation();

				if ( parentBoneId >= 0 )
				{
					const FTransform& parentBoneTransform = skelState.WorldSpaceBoneTransforms[parentBoneId];
					Start = parentBoneTransform.GetLocation();
					LineColor = FColor::White;
				}
				else
				{
					Start = GetTransform().GetLocation();
					LineColor = FColor::Red;
				}

				static const float SphereRadius = 2.0f;

				FVector EndToStart = (Start - End);
				float ConeLength = EndToStart.Size();
				float Angle = FMath::RadiansToDegrees( FMath::Atan( SphereRadius / ConeLength ) ) * 0.02f;

				DrawDebugSphere( GetWorld(), End, SphereRadius, 10, LineColor );
				DrawDebugCone( GetWorld(), End, EndToStart, ConeLength, Angle, Angle, 4, LineColor );
				//DrawDebugCoordinateSystem( GetWorld(), End, boneTransform.Rotator(), SphereRadius * 4.0f, false, -1.f, 0.1f );
				//DrawDebugString( GetWorld(), End, boneDef.Name.ToString(), nullptr, FColor::White, 0.f, true );
			}
		}
	}
    */
	FrameUpdateLock.Unlock();
}


bool AMRICaptureVolume::ToggleTracking()
{
    if( !IsTracking )
    {
       // auto teid = CAPI_CreateTrackEntity( TrackObjectId, TrackInstanceId ); //!!!

        //if( Skeleton )
        //{
        //    if( _indexedTransforms.Count == 0 )
        //    {
        //        InitializeSkeleton();
        //    }
        //}

        IsTracking = CAPI_StartTracking();
    }
    else
    {
        CAPI_StopAllTracking();
        IsTracking = false;
    }
    
    return IsTracking;
}


//static
AMRICaptureVolume* AMRICaptureVolume::FindDefaultCaptureVolume( UWorld* World )
{
    AMRICaptureVolume* returnCaptureVolume = nullptr;

    int numCaptureVolumes = 0;
    for( TActorIterator<AMRICaptureVolume> VolumeIter( World ); VolumeIter; ++VolumeIter )
    {
        ++numCaptureVolumes;

        if( numCaptureVolumes == 1 )
        {
            returnCaptureVolume = *VolumeIter;
        }
    }

    if( numCaptureVolumes == 0 )
    {
        UE_LOG( LogTemp, Warning, TEXT( "AMRICaptureVolume::FindDefaultCaptureVolume: Unable to locate any client origin actors!" ) );
    }
    else if( numCaptureVolumes > 1 )
    {
        UE_LOG( LogTemp, Warning, TEXT( "AMRICaptureVolume::FindDefaultCaptureVolume: Multiple client origin actors; using first (%s)" ), *returnCaptureVolume->GetName() );
    }

    return returnCaptureVolume;
}



bool AMRICaptureVolume::GetLatestSkeletonState( int32 SkelId, FMRISkeletonState& OutSkelState )
{
    bool bResult = false;

    // NOTE: This lock blocks NatNet frame delivery for the duration.
    // See comment in NatNetDataCallback.
    FrameUpdateLock.Lock();

    if( const FMRISkeletonState* pState = LatestSkeletonStates.Find( SkelId ) )
    {
        OutSkelState = *pState;
        bResult = true;
    }

    FrameUpdateLock.Unlock();

    return bResult;
}

bool AMRICaptureVolume::FindSkeletonDefinition( const FName& SkeletonName, FMRISkeletonDefinition& OutSkelDef )
{
    for( const TPair< int32, FMRISkeletonDefinition >& SkelDef : SkeletonDefinitions )
    {
        if( SkelDef.Value.Name == SkeletonName )
        {
            OutSkelDef = SkelDef.Value;
            return true;
        }
    }

    return false;
}

/*
bool AMRICaptureVolume::InitializeClient()
{
	check( IOptitrackNatnetPlugin::IsAvailable() );

	if ( Client != nullptr )
	{
		UE_LOG( LogOptitrack, Error, TEXT( "AMRICaptureVolume: Not valid to call InitializeClient when already initialized" ) );
		return false;
	}

	Client = IOptitrackNatnetPlugin::Get().CreateClient();

	if ( Client == nullptr )
	{
		UE_LOG( LogOptitrack, Error, TEXT( "AMRICaptureVolume: IOptitrackNatnetPlugin::CreateClient failed" ) );
		return false;
	}

	const std::string strClientAddr( TCHAR_TO_ANSI( *ClientAddress ) );
	const std::string strServerAddr( TCHAR_TO_ANSI( *ServerAddress ) );

	sNatNetClientConnectParams connectParams;
	connectParams.connectionType = (ConnectionType == EOptitrackClientConnectionType::Unicast) ? ConnectionType_Unicast : ConnectionType_Multicast;
	connectParams.localAddress = strClientAddr.c_str();
	connectParams.serverAddress = strServerAddr.c_str();
	const ErrorCode ConnectResult = Client->Connect( connectParams );
	if ( ConnectResult != ErrorCode_OK )
	{
		UE_LOG( LogOptitrack, Error, TEXT( "AMRICaptureVolume: NatNetClient::Connect failed with return code %d" ), static_cast<int32>(ConnectResult) );
		ShutdownClient();
		return false;
	}
	sDataDescriptions* DataDescriptions = nullptr;
	const ErrorCode GetDataDescsResult = Client->GetDataDescriptionList( &DataDescriptions );
	if ( !ensureMsgf( GetDataDescsResult == ErrorCode_OK && DataDescriptions != nullptr, TEXT( "NatNetClient::GetDataDescriptionList failed with return code %d" ), GetDataDescsResult ) )
	{
		ShutdownClient();
		return false;
	}
// Parse data descriptions for skeleton definitions.
	const float coordScalingFactor = CachedWorldToMeters;
	for ( int32 descIdx = 0; descIdx < DataDescriptions->nDataDescriptions; ++descIdx )
	{
		if ( DataDescriptions->arrDataDescriptions[descIdx].type == Descriptor_Skeleton )
		{
			const sSkeletonDescription& skelDesc =
				*DataDescriptions->arrDataDescriptions[descIdx].Data.SkeletonDescription;

			FOptitrackSkeletonDefinition skelDef;
			skelDef.Id = skelDesc.skeletonID;
			skelDef.Name = skelDesc.szName;

			skelDef.Bones.SetNumUninitialized( skelDesc.nRigidBodies + 1 );
			skelDef.Bones[0].Id = 0;
			skelDef.Bones[0].LocalOffset = FVector::ZeroVector;
			skelDef.Bones[0].Name = "Root";
			skelDef.Bones[0].ParentId = -1;

			for ( int32 boneIdx = 0; boneIdx < skelDesc.nRigidBodies; ++boneIdx )
			{
				const sRigidBodyDescription& boneDesc = skelDesc.RigidBodies[boneIdx];
				checkSlow( boneDesc.ID > 0 && boneDesc.ID < skelDef.Bones.Num() );

				FOptitrackBoneDefinition& boneDef = skelDef.Bones[ boneDesc.ID ];
				boneDef.Name = boneDesc.szName;
				boneDef.Id = boneDesc.ID;
				boneDef.ParentId = boneDesc.parentID;
				// FIXME: Motive streams out skeleton definitions with inverted X coordinates.
				boneDef.LocalOffset = FVector( -boneDesc.offsetx * coordScalingFactor, boneDesc.offsetz * coordScalingFactor, boneDesc.offsety * coordScalingFactor );
			}

			SkeletonDefinitions.Emplace( skelDef.Id, MoveTemp( skelDef ) );
		}
	}
	Client->SetFrameReceivedCallback( &NatNetFrameReceivedThunk, this );
    sDataDescriptions* pDataDefs = NULL;
    Client->GetDataDescriptionList(&pDataDefs);

    // Set local/global streaming setting as well as match bone naming convention
    SetDefaultSkeletonSettings();
    
	return true;
}


bool AMRICaptureVolume::ShutdownClient()
{
	if ( Client != nullptr )
	{
		FrameUpdateLock.Lock();

		//Client->Disconnect();
		//check( IOptitrackNatnetPlugin::IsAvailable() );
		//IOptitrackNatnetPlugin::Get().DestroyClient( Client );
		//Client = nullptr;
		//TimestampType = EOptitrackTimestampType::Unknown;

		FrameUpdateLock.Unlock();

		return true;
	}
	else
	{
		UE_LOG( LogOptitrack, Error, TEXT( "AMRICaptureVolume: Not valid to call ShutdownClient when not initialized" ) );
		return false;
	}
	SkeletonDefinitions.Reset();
}

*/