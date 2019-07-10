// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using Path = System.IO.Path;

public class MRITrackPlugin : ModuleRules
{
	public MRITrackPlugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"MRITrackPlugin/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"MRITrackPlugin/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				//"MRITrackPluginLibrary",
				"Projects",
                "CoreUObject",
                "Engine",
                "Networking",
                "Sockets",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        string TrackNetClientPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty", "MRI"));
        PublicSystemIncludePaths.Add(Path.Combine(TrackNetClientPath, "include"));

        if (Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64)
        {
            string TrackNetClientBinPath = Path.Combine(TrackNetClientPath, "lib", "Win64");
            //Log.TraceInformation(TrackNetClientBinPath);
            PublicLibraryPaths.Add(TrackNetClientBinPath);
            PublicAdditionalLibraries.Add("TrackNetClient.lib");
            PublicDelayLoadDLLs.Add("TrackNetClient.dll");
            RuntimeDependencies.Add(Path.Combine(TrackNetClientBinPath, "TrackNetClient.dll"));

            //PublicAdditionalLibraries.Add("HmdDriftCorrection.dll");
            PublicDelayLoadDLLs.Add("HmdDriftCorrection.dll");
            RuntimeDependencies.Add(Path.Combine(TrackNetClientBinPath, "HmdDriftCorrection.dll"));
        }

    }
}
