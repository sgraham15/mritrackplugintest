
#pragma once

#include <AnimGraphNode_Base.h>
#include "AnimNode_MRISkeleton.h"

#include "AnimGraphNode_MRISkeleton.generated.h"


UCLASS( BlueprintType )
class UAnimGraphNode_MRISkeleton : public UAnimGraphNode_Base
{
    GENERATED_UCLASS_BODY()

    UPROPERTY( EditAnywhere, Category="Settings" )
    FAnimNode_MRISkeleton Node;

public:
    virtual FText GetNodeTitle( ENodeTitleType::Type TitleType ) const override;
    virtual FText GetTooltipText() const override;
    virtual FText GetMenuCategory() const override;
};
