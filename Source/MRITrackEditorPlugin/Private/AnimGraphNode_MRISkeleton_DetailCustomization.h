#pragma once

#if 1
#include <CoreMinimal.h>
#include <IDetailCustomization.h>
#include <Input/Reply.h>

class IDetailLayoutBuilder;
class IPropertyHandle;
class UAnimGraphNode_MRISkeleton;


class FAnimGraphNode_MRISkeleton_DetailCustomization : public IDetailCustomization
{
public:
	static TSharedRef< IDetailCustomization > MakeInstance();

	virtual void /*IDetailCustomization::*/CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

private:
	void OnGenerateElementForBoneMapping( TSharedRef<class IPropertyHandle> ElementProperty, int32 ElementIndex, class IDetailChildrenBuilder& ChildrenBuilder );

	bool IsEnabled_UpdateRetargetBasePose() const;
	FText GetText_UpdateRetargetBasePose() const;
	FReply OnClicked_UpdateRetargetBasePose();

    bool IsEnabled_AutoFillBoneMapping() const { return true; }
    FText GetText_AutoFillBoneMapping() const;
    FReply OnClicked_AutoFillBoneMapping() const;

	UAnimGraphNode_MRISkeleton* GraphNode;
};

#endif