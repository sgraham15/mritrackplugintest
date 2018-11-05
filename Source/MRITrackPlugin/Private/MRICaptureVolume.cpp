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
        
#define TEST_OCULUS 1

#if TEST_OCULUS
        if( b )
        {
            const int instanceId = 0;
            const int hmd_toid = 42;

            auto teid = CAPI_CreateTrackEntity(hmd_toid, instanceId);
            InstanceIds.Add(instanceId);
            ToggleTracking();
        }
        else
        {
            checkf(false, TEXT("Failed to initialize tracking"));
        }
#endif
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

//bool AMRICaptureVolume::GetLatestHMDPosition( int instanceId, FVector & position )
//{
//    if( !CheckOrInitHmdBone( instanceId ) )
//        return false;
//        
//    auto quatVec = CAPI_GetBonePoseAbsolute( instanceId, HMDBoneId );
//    position.Set( quatVec.v.x, quatVec.v.y, quatVec.v.z );
//    return true;
//}
//
//bool AMRICaptureVolume::GetLatestHMDRotation( int instanceId, FRotator & rotation )
//{
//    if (!CheckOrInitHmdBone(instanceId))
//        return false;
//
//    auto quatVec = CAPI_GetBonePoseAbsolute( instanceId, HMDBoneId );
//    rotation = FRotator( FQuat(quatVec.q.x, quatVec.q.y, quatVec.q.z, quatVec.q.w) );
//    return true;
//}


bool AMRICaptureVolume::GetLatestHmdTransform(int instanceId, FVector & position, FQuat & rotation)
{
    if ( !InstanceIds.Contains( instanceId ) || !IsTracking )
        return false;
    if (!CheckOrInitHmdBone(instanceId))
        return false;

    auto quatVec = CAPI_GetBonePoseAbsolute( instanceId, HMDBoneId );
    position.Set( quatVec.v.x, quatVec.v.y, quatVec.v.z );
    rotation = FQuat(quatVec.q.x, quatVec.q.y, quatVec.q.z, quatVec.q.w);

    {
        // Hack.
        FMatrix mtx;
        //FVector xaxis( 1.0f, 0.0f, 0.0f );
        //FVector yaxis( 0.0f, 0.0f, -1.0f );
        //FVector zaxis( 0.0f, 1.0f, 0.0f );
        // Working!!!
        FVector xaxis1(0.0f, 1.0f, 0.0f);
        FVector yaxis1(0.0f, 0.0f, 1.0f);
        FVector zaxis1(-1.0f, 0.0f, 0.0f);
        mtx.SetAxes(&xaxis1, &yaxis1, &zaxis1);


        FVector xaxis(0.0f, 1.0f, 0.0f);
        FVector yaxis(0.0f, 0.0f, 1.0f);
        FVector zaxis(-1.0f, 0.0f, 0.0f);

        
        FVector origin(0,0,0);
       // FTransform xform(xaxis, yaxis, zaxis, origin);  //!!!! this expression breaks our entire view somehow??? what the fuck
        
        
        //FTransform xform( mtx );
        //position = xform.TransformPosition( position );
        //rotation = xform.TransformRotation( rotation );

        //q = q.Inverse();
        //rotation = q * rotation;
        //position = q * position;

        position = mtx.TransformPosition(position);
        //position /= 100.0f;


        ///auto det = mtx.RotDeterminant();
        //FQuat q(mtx.Rotator());
        //q.Normalize();
        //rotation = q * rotation;
    }

    return true;
}


bool AMRICaptureVolume::CheckOrInitHmdBone(int instanceId)
{
    if( HMDBoneId >= 0 )
        return true;

    int boneId = CAPI_GetBoneIndexByName(instanceId, "HMD");
    if (boneId >= 0)
    {
        HMDBoneId = boneId;
        return true;
    }

    return false;
}

TrackEntityIdWrapper AMRICaptureVolume::ConvertGuidToTeid(const FGuid & guid)
{
    TrackEntityIdWrapper teid;
    teid.e0 = guid[0];
    teid.e1 = guid[1];
    teid.e2 = guid[2];
    teid.e3 = guid[3];
    return teid;
}

FGuid AMRICaptureVolume::ConvertTeidToGuid(const TrackEntityIdWrapper & teid)
{
    FGuid guid(teid.e0, teid.e1, teid.e2, teid.e3);
    return guid;
}

#if Ref

template <typename VictoryObjType> 
static FORCEINLINE VictoryObjType* SpawnBP( UWorld* TheWorld,
                                            UClass* TheBP,
                                            const FVector& Loc,
                                            const FRotator& Rot,
                                            const bool bNoCollisionFail = true,
                                            AActor* Owner = nullptr,
                                            APawn* Instigator = nullptr ) 
{
    // https://wiki.unrealengine.com/Templates_in_C%2B%2B
    if( !TheWorld ) 
        return nullptr; 
    if( !TheBP ) 
        return nullptr;

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.bNoCollisionFail = bNoCollisionFail;
    SpawnInfo.Owner = Owner;
    SpawnInfo.Instigator = Instigator;
    SpawnInfo.bDeferConstruction = false;

    return TheWorld->SpawnActor<VictoryObjType>( TheBP, Loc, Rot, SpawnInfo );
}


//{
//    AActorBaseClass* NewActor = UFunctionLibrary::SpawnBP<AActorBaseClass>( GetWorld(), TheActorBluePrint, SpawnLoc, SpawnRot );
//
//    AActorBaseClass* NewActor = SpawnBP<AActorBaseClass>( GetWorld(), TheActorBluePrint, SpawnLoc, SpawnRot );
//}

void FindBP()
{
    auto const path = TEXT( "Blueprint'/Game/Items/Blueprints/BP_ItemTest.BP_ItemTest'" );
    static ConstructorHelpers::FObjectFinder<UBlueprint> ItemBlueprint( path );
    if( ItemBlueprint.Object ) 
    {
        MyItemBlueprint = ( UClass* ) ItemBlueprint.Object->GeneratedClass;
    }
 }

void SpawnBP()
{
    //https://answers.unrealengine.com/questions/53689/spawn-blueprint-from-c.html
    UWorld* const World = GetWorld();
    if( World )
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Instigator = this;
        AItem* DroppedItem = World->SpawnActor<ASurItem>( MyItemBlueprint, Location, Rotation, SpawnParams );
        //if( DroppedItem )
        //{
        //    DroppedItem->DoTheThings();
        //}
    }

}

should just have a blue print map node of some sort to allow users to plug in their bp's?

void ListAllBlueprintsInPath( FName Path, TArray<UClass*>& Result, UClass* Class )
{
    //https://answers.unrealengine.com/questions/89482/dynamic-blueprints-loading.html#answer-309155
    auto Library = UObjectLibrary::CreateLibrary( Class, true, GIsEditor );
    Library->LoadBlueprintAssetDataFromPath( Path.ToString() );

    TArray<FAssetData> Assets;
    Library->GetAssetDataList( Assets );

    for( auto& Asset : Assets )
    {
        UBlueprint* bp = Cast<UBlueprint>( Asset.GetAsset() );
        if( bp )
        {
            auto gc = bp->GeneratedClass;
            if( gc )
            {
                Result.Add( gc );
            }
        }
        else
        {
            auto GeneratedClassName = ( Asset.AssetName.ToString() + "_C" );

            UClass* Clazz = FindObject<UClass>( Asset.GetPackage(), *GeneratedClassName );
            if( Clazz )
            {
                Result.Add( Clazz );
            }
            else
            {
                UObjectRedirector* RenamedClassRedirector = FindObject<UObjectRedirector>( Asset.GetPackage(), *GeneratedClassName );
                if( RenamedClassRedirector )
                {
                    Result.Add( CastChecked<UClass>( RenamedClassRedirector->DestinationObject ) );
                }
            }

        }
    }

    return true;
}

#endif

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