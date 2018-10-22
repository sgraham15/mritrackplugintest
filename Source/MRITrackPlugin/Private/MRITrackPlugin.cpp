// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "MRITrackPlugin.h"
#include <stdint.h>
#include "Core.h"
#include "ModuleManager.h"
#include "IPluginManager.h"
#include "ExampleLibrary.h"

#define LOCTEXT_NAMESPACE "FMRITrackPluginModule"


void FMRITrackPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("MRITrackPlugin")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
    LibraryPath = FPaths::Combine( *BaseDir, TEXT( "ThirdParty/MRI/lib/Win64/TrackNetClient.dll" ) );//"Binaries/ThirdParty/MRITrackPluginLibrary/x64/Release/TrackNetClient.dll"));
//#elif PLATFORM_MAC
//    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/MRITrackPluginLibrary/Mac/Release/libExampleLibrary.dylib"));
#endif // PLATFORM_WINDOWS

	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (ExampleLibraryHandle)
	{
        FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT( "ThirdPartyLibraryError", "Succeeded in loading library." ) );
		// Call the test function in the third party library that opens a message box
		//ExampleLibraryFunction();
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load example third party library"));
	}
}

void FMRITrackPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMRITrackPluginModule, MRITrackPlugin)