#pragma once

#include <GameFramework/Actor.h>

#include "MRISkeletonTypes.h"

#include "MRICaptureVolume.generated.h"


// forward decl
struct TrackEntityIdWrapper;


UENUM( BlueprintType )
enum class EMRITimestampType : uint8
{
    // We don't know what the server supports until after we've received at least one frame.
    Unknown = 0,

    // When the NatNet frame callback occurred. Fallback when precise metrics are unavailable.
    LocalArrivalTime,

    // Clock synchronized estimate of when the camera data became available on the server. 
    ServerAggregationTime,

    // Clock synchronized estimate of when the camera exposure occurred. 
    ServerExposureTime,
};


UCLASS()
class MRITRACKPLUGIN_API AMRICaptureVolume : public AActor
{
    GENERATED_UCLASS_BODY()

    // The network address of the NatNet server to connect to.
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="MRI" )
    FString ServerAddress;

    // The address of the local network interface to use. 
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="MRI" )
    FString ClientAddress;

    // The NatNet client type to instantiate. Must match the setting of the NatNet server.
    //UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="MRI" )
    //EMRIClientConnectionType ConnectionType;

    //UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="MRI Connection Settings" )
    //EMRIBoneNamingConvention BoneNamingConvention;

    // If non-zero, the rigid body with the specified ID is used to provide input to compatible HMD implementations.
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="MRI" )
    int32 HmdRigidBodyId;

    // If true, call InitializeClient and connect automatically during PreInitializeComponents. 
    UPROPERTY(EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = "MRI")
    bool bAutoInitialize = true;

    // If true, draw Motive and UE bone locations (white and green respectively)
    UPROPERTY( EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category="MRI" )
    uint8 bDrawDebugSkeletons : 1;

    // List of available actor blueprints we can spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
    TArray<TSubclassOf<class AActor>> SpawnActors;

    // Orientation of HMD in tracking volume 
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MRI")
    //EMRIForwardAxisType HmdOrientation = EMRIForwardAxisType::Z_Positive;

    /**
     * Retrieves latest available FMRIRigidBodyState for the rigid body identified by the
     * @RbId parameter. Note that this data can be stale if the rigid body in question hasn't
     * streamed any new data recently.
     *
     * The tracking space pose is transformed by the offset/rotation/scale of this actor,
     * such that the placement of this actor corresponds to the tracking space origin.
     *
     * @param OutRbState Receives latest available rigid body state (if any).
     * @return True if any rigid body state was available for the specified ID,
     *         otherwise false.
     */
    //UFUNCTION( BlueprintCallable, Category="MRI" )
    //bool GetLatestRigidBodyState( int32 RbId, FMRIRigidBodyState& OutRbState );

    /**
    * TODO
    *
    * @param OutSkelState
    * @return True if any skeleton state was available for the specified ID,
    *         otherwise false.
    */
    UFUNCTION(BlueprintCallable, Category = "MRI|Advanced")
    bool GetLatestSkeletonState(int32 SkelId, FMRISkeletonState& OutSkelState);

    UFUNCTION(BlueprintCallable, Category = "MRI|Advanced")
    bool FindSkeletonDefinition(const FName& SkeletonName, FMRISkeletonDefinition& OutSkelDef);

    bool HasCreatedTrackClientFunc() const { return HasCreatedTrackClient; };
    bool HasSetRemoteFunc() const { return HasSetRemote; };

    /**
     * Retrieves latest available FMRIRigidBodyState for the rigid body identified by the
     * @RbId parameter. Note that this data can be stale if the rigid body in question hasn't
     * streamed any new data recently.
     *
     * This "Untransformed" version returns the tracking space pose, applying only the global
     * WorldToMeters scale. It does not take into account the transform of this actor.
     *
     * @param OutRbState Receives latest available rigid body state (if any).
     * @return True if any rigid body state was available for the specified ID,
     *         otherwise false.
     */
    //UFUNCTION( BlueprintCallable, Category="MRI|Advanced" )
    //bool GetLatestRigidBodyStateUntransformed( int32 RbId, FMRIRigidBodyState& OutRbState );

    /** Describes the semantics of @FMRIRigidBodyState::TimestampPlatformSeconds values. */
    //UFUNCTION( BlueprintCallable, Category="MRI" )
    //EMRITimestampType GetTimestampType() const { return TimestampType; }

    /**
     * Helper for the common case where only a single client origin is present.
     * Used to provide a default for any UMRIRigidBody components which
     * don't otherwise specify a particular origin to use.
     * Note: This is potentially expensive and the result should be cached.
     * @return The first AMRIClientOrigin actor found in @World.
     */
    public:
    UFUNCTION( BlueprintCallable, Category="MRI" )
    static AMRICaptureVolume* FindDefaultCaptureVolume( UWorld* World );

    /**
     * Finds the first AMRIClientOrigin specifying an HMD rigid body ID.
     * Note: This is potentially expensive and the result should be cached.
     * @return The first suitable AMRIClientOrigin actor found in @World.
     */
    //UFUNCTION( BlueprintCallable, Category="MRI" )
    //static AMRIClientOrigin* FindHmdClientOrigin( UWorld* World );

    /**
     * Returns true if the last call to InitializeClient succeeded,
     * and ShutdownClient has not been called subsequently.
     */
    //UFUNCTION( BlueprintCallable, Category="MRI" )
    //bool IsClientInitialized() const { return Client != nullptr; }

    /**
     * Attempt to initialize NatNet and connect to the server.
     * Invalid to call if @IsClientInitialized() returns true.
     * @return True on success, false on failure.
     */
    //UFUNCTION( BlueprintCallable, Category="MRI" )
    //bool InitializeClient();

    /**
     * Attempt to shut down any existing NatNet client.
     * Invalid to call if @IsClientInitialized() returns false.
     * @return True on success, false on failure.
     */
    //UFUNCTION( BlueprintCallable, Category="MRI" )
    //bool ShutdownClient();

public:
    //void NatNetFrameReceivedCallback_Internal( struct sFrameOfMocapData* NewFrame );
    //void ValidateAuthenticationToken(const char* challengeToken, char* authToken);

    // make these into UFUNCTIONs for blueprint callability?
    //bool GetLatestHMDPosition(int instanceId, FVector & position );
    //bool GetLatestHMDRotation(int instanceId, FRotator & rotation );

    bool GetLatestHmdTransform(int instanceId, FVector & position, FQuat & rotation);

    static TrackEntityIdWrapper ConvertGuidToTeid( const FGuid & guid );
    static FGuid ConvertTeidToGuid( const TrackEntityIdWrapper & teid );

protected:
    //~ Begin AActor Interface
    //virtual void PreInitializeComponents() override;
    virtual void BeginPlay();
    virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;
    virtual void Tick(float DeltaSeconds) override;

private:
    bool SetDefaultSkeletonSettings();
    bool ToggleTracking();

    bool CheckOrInitHmdBone( int instanceId );

    bool SetRemote();

    //class NatNetClient* Client = nullptr;

    /** Controls access to @LatestRigidBodyStates map. */
    FCriticalSection FrameUpdateLock;

    /** Copied from AWorldSettings::WorldToMeters for use in the NatNet callback (which happens on another thread). */
    //float CachedWorldToMeters;
    bool HasCreatedTrackClient{ false };
    bool HasSetRemote{ false };
    bool IsTracking{ false };
    int TrackObjectId{ 200 };
    int TrackInstanceId{ -1 };
    TArray<int32> InstanceIds;
    float LastSetRemoteAttemptSeconds{ 0.0f };
    int CurrInstanceId{ 0 };
    int HMDBoneId{ -1 };
    
    UPROPERTY()
    TMap< int32, FMRISkeletonDefinition > SkeletonDefinitions;

    /** Key is rigid body ID, value is latest pose available for that rigid body. */
    //UPROPERTY()
    //TMap< int32, FMRIRigidBodyState > LatestRigidBodyStates;

    /** Key is skeleton ID, value is latest set of bone poses available for that skeleton. */
    UPROPERTY()
    TMap< int32, FMRISkeletonState > LatestSkeletonStates;

    EMRITimestampType TimestampType = EMRITimestampType::Unknown;
};
