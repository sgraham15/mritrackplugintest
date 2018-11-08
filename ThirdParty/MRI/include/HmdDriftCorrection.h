#pragma once

#if defined( __cplusplus )
extern "C" {
#endif

#define MRI_IMPORT __declspec(dllimport)

enum NpHmdResult 
{
	OK = 0,
	InvalidArgument
};

struct NpHmdQuaternion
{
	float x;
	float y;
	float z;
	float w;
};

//MRI_IMPORT NpHmdResult NpHmd_Create( void ** hmdHandle );
//MRI_IMPORT NpHmdResult NpHmd_Destroy( void * hmdHandle );
//
//MRI_IMPORT NpHmdResult NpHmd_MeasurementUpdate( void * hmdHandle, 
//								 	 NpHmdQuaternion * opticalOrientation, 
//								     NpHmdQuaternion * inertialOrientation, 
//									 float delta );
//
//MRI_IMPORT NpHmdResult NpHmd_GetOrientationCorrection( void * hmdHandle, NpHmdQuaternion * outOrientation );

#if defined( __cplusplus )
} // extern "C"
#endif
