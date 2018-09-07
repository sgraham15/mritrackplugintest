
namespace UnrealBuildTool.Rules
{
	public class MRITrackEditorPlugin : ModuleRules
	{
        public MRITrackEditorPlugin( ReadOnlyTargetRules Target ) : base( Target )
		{
			PrivateIncludePaths.Add( "MRITrackEditorPlugin/Private" );
			PublicIncludePaths.Add( "MRITrackEditorPlugin/Public" );

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"AnimGraphRuntime",

					"UnrealEd",
					"BlueprintGraph",
					"AnimGraph",
					"SlateCore",
					"Slate",
					"EditorStyle",
					"PropertyEditor",

					"MRITrackPlugin",
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{

				}
			);
		}
	}
}
