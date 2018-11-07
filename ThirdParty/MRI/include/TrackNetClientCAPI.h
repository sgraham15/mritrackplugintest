#pragma once


#if defined( __cplusplus )
extern "C" {
#endif

struct Vector3f
{
    float x;
    float y;
    float z;
};

struct Quaternion
{
    float x;
    float y;
    float z;
    float w;
};

struct QuatVec3f
{
    Quaternion q;
    Vector3f v;
};

typedef uint32_t TrackObjID;

enum ITrackVersion { ITRACK_VERSION_NUM = 18 };

enum ConversionDescriptorMode
{
    RAW,
    CRYENGINE,
    UNITY,
    MOTIONBUILDER,
    UNREALENGINE
};

struct StartingPosition
{
    int id;
    Vector3f position;
};

typedef uint32_t ScalingSessionID;


enum CAPI_TRACKNET_RESPONSE
{
    e_ResponseCallbackRegistered = 0,
    e_ClientCreation_Success,
    e_ClientCreation_Failure,
    e_ClientConnect_Success,
    e_ClientConnect_Failure,
    e_ClientShutdown_Success,
    e_ClientShutdown_Failure,
    e_ScalingCallbackRegistered,
    e_ScalingStarted,
    e_ScalingComplete,
    e_ScalingAccepted,
    e_GeneratedScalingResult,
    e_ScalingMarkerResultCallback,
    e_ScalingTestResultCallback,
    e_SetFromTrackEntityId_Success,
    e_SetFromTrackEntityId_Failure,
    e_TrackRosterEmpty,
};

struct TrackEntityIdWrapper
{
    unsigned int e0, e1, e2, e3;
};

typedef void( __stdcall *TrackNetResponse_Callback )( CAPI_TRACKNET_RESPONSE );
//typedef void( __stdcall *ScalingComplete_Callback )( ScalingSessionID );
//typedef void( __stdcall *ScalingMarkersRequest_Callback )( ScalingMarker );
//typedef void( __stdcall *ScalingTestResultsRequest_Callback )( IScalingSessionResult::TestResultData );


//#   if defined( NATNETLIB_EXPORTS )
//#       define NATNET_API               __declspec(dllexport)
//#   elif defined( NATNETLIB_IMPORTS )
//#       define NATNET_API               __declspec(dllimport)
//#   else
//#       define NATNET_API
//#   endif

#define TRACKNETCLIENT_API               __declspec(dllimport)

TRACKNETCLIENT_API void CAPI_SetResponseCallback( TrackNetResponse_Callback callback );
//TRACKNETCLIENT_API void CAPI_SetScalingCompleteCallback( ScalingComplete_Callback callback );
//TRACKNETCLIENT_API void CAPI_SetScalingMarkersRequestCallback( ScalingMarkersRequest_Callback callback );
//TRACKNETCLIENT_API void CAPI_SetScalingTestResultsRequestCallback( ScalingTestResultsRequest_Callback callback );

TRACKNETCLIENT_API bool CAPI_IsServerReachable();
TRACKNETCLIENT_API bool CAPI_CreateTrackClient( const char *dllPath, ITrackVersion version, ConversionDescriptorMode convDescMode );
TRACKNETCLIENT_API bool CAPI_SetRemote( const char * rpcUrl, const char * hostname, const char * auxHostname );
TRACKNETCLIENT_API void CAPI_EliminateNoise();
TRACKNETCLIENT_API bool CAPI_StartTrackingEntities( TrackEntityIdWrapper * entities, int size );
TRACKNETCLIENT_API void CAPI_StartTrackingEntitiesByInstanceId( int * instanceIds, int size );
TRACKNETCLIENT_API bool CAPI_StartTracking();
TRACKNETCLIENT_API bool CAPI_StopAllTracking();
TRACKNETCLIENT_API bool CAPI_ApplyLatestServerData( int renderFrameNum );
TRACKNETCLIENT_API TrackEntityIdWrapper CAPI_CreateTrackEntity( int objectId, int instanceId );
TRACKNETCLIENT_API bool CAPI_SetTrackEntityById( int instanceId, int toid, TrackEntityIdWrapper teid );
TRACKNETCLIENT_API int CAPI_GetBoneIndexByName( int instanceId, const char *boneName );

TRACKNETCLIENT_API int CAPI_GetBoneCount( int instanceId );
TRACKNETCLIENT_API const char * CAPI_GetBoneNameByIndex( int instanceId, int boneIndex );
TRACKNETCLIENT_API QuatVec3f CAPI_GetBonePoseAbsolute( int instanceId, int boneIndex );
TRACKNETCLIENT_API QuatVec3f CAPI_GetBonePoseAbsoluteRaw( int instanceId, int boneIndex );
TRACKNETCLIENT_API QuatVec3f CAPI_GetBonePoseRelative( int instanceId, int boneIndex );
TRACKNETCLIENT_API void CAPI_GetAllBonePosesRelative( int instanceId, QuatVec3f **poses, int32_t* boneCount );
TRACKNETCLIENT_API void CAPI_GetAllBonePosesAbsolute( int instanceId, QuatVec3f **poses, int32_t *boneCount );
TRACKNETCLIENT_API QuatVec3f CAPI_GetBonePoseRelativeRaw( int instanceId, int boneIndex );
TRACKNETCLIENT_API void CAPI_SetStartingPose( int instanceId, const char *poseName, Vector3f pos, float rotationRadians );
TRACKNETCLIENT_API void CAPI_SetStartingMating( int instanceId, int hostInstanceId, const char *hostBone, const char *slaveBone );
TRACKNETCLIENT_API bool CAPI_RequestRoster();
TRACKNETCLIENT_API const char * CAPI_GetBoneMappingJSON( int instanceId, const char *mappingType );

TRACKNETCLIENT_API void CAPI_RequestStartingPositions( StartingPosition ** positions, int * count );
TRACKNETCLIENT_API int CAPI_GetScalingObjectIDsCount( bool doRefresh );
TRACKNETCLIENT_API const char *CAPI_GetScalingObjectID( int index );

TRACKNETCLIENT_API int CAPI_GetRosterSize();
TRACKNETCLIENT_API int CAPI_GetNumLocalEntities();
//! @note Use this to get an array of track entities that TrackClient knows about, but the CAPI layer hasn't registered yet.
//!       Ideally we could get rid of this if we stop doing things by instanceId cache which is beyond stupid in the first place.
TRACKNETCLIENT_API void CAPI_GetUninitializedTrackEntities(int * count, int ** toids, TrackEntityIdWrapper ** teids);
TRACKNETCLIENT_API void CAPI_SyncUnregisteredEntities();
TRACKNETCLIENT_API int CAPI_GetRegisteredEntityCount();
TRACKNETCLIENT_API int CAPI_GetUnregisteredEntityCount();

//scaling -- Doesn't work....
//TRACKNETCLIENT_API void CAPI_Scale( ScalingRequest request );
//TRACKNETCLIENT_API TrackObjID CAPI_AcceptScaling( ScalingSessionID scalingSessionId );
//TRACKNETCLIENT_API void CAPI_GenerateScalingResult( int instanceId, ScalingSessionID scalingSessionId );
//TRACKNETCLIENT_API void CAPI_RequestScalingResultMarkers( int instanceId ); //instanceId = ScalingResultTest id from CAPI_GenerateScalingResult
//TRACKNETCLIENT_API void CAPI_RequestScalingTestResults( int instanceId ); //instanceId = ScalingResultTest id from CAPI_GenerateScalingResult
//TRACKNETCLIENT_API int CAPI_GetNumScalingMarkers(); //instanceId = ScalingResultTest id from CAPI_GenerateScalingResult
//TRACKNETCLIENT_API ScalingMarker CAPI_GetScalingMarker( int index ); //instanceId = ScalingResultTest id from CAPI_GenerateScalingResult
//TRACKNETCLIENT_API int CAPI_GetNumScalingTestResults(); //instanceId = ScalingResultTest id from CAPI_GenerateScalingResult
//TRACKNETCLIENT_API IScalingSessionResult::TestResultData CAPI_GetScalingTestResult( int index ); //instanceId = ScalingResultTest id from CAPI_GenerateScalingResult
//TRACKNETCLIENT_API bool CAPI_WasScalingSuccessful( int instanceId ); //instanceId = ScalingResultTest id from CAPI_GenerateScalingResult
//TRACKNETCLIENT_API void CAPI_ReleaseScalingResult( int instanceId );

TRACKNETCLIENT_API bool CAPI_Shutdown();


#if defined( __cplusplus )
} // extern "C"
#endif