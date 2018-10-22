#include "AnimGraphNode_MRISkeleton.h"
#include "MRITrackEditorPlugin.h" //#include "OptitrackNatnetEditor.h"
//#include "AnimGraphNode_MRISkeleton.h"
#include "AnimGraphNode_MRISkeleton_DetailCustomization.h"
#include <PropertyEditorModule.h>


class FMRIEditorModule : public IMRITrackEditorPlugin
{
public:
    // IModuleInterface implementation
	void StartupModule() override;
	void ShutdownModule() override;
};


IMPLEMENT_MODULE( FMRIEditorModule, MRITrackEditorPlugin )


void FMRIEditorModule::StartupModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
	PropertyModule.RegisterCustomClassLayout(
		UAnimGraphNode_MRISkeleton::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic( &FAnimGraphNode_MRISkeleton_DetailCustomization::MakeInstance )
	);
}


void FMRIEditorModule::ShutdownModule()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
	PropertyModule.UnregisterCustomClassLayout( "AnimGraphNode_MRISkeleton" );
}
