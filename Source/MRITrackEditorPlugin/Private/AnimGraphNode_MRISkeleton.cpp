/*
Copyright 2018 NaturalPoint

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#include "AnimGraphNode_MRISkeleton.h"


#define LOCTEXT_NAMESPACE "MRINatnet"


UAnimGraphNode_MRISkeleton::UAnimGraphNode_MRISkeleton( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
}


FText UAnimGraphNode_MRISkeleton::GetNodeTitle( ENodeTitleType::Type TitleType ) const
{
    return LOCTEXT( "MRISkeleton_NodeTitle", "MRI Skeleton" );
}


FText UAnimGraphNode_MRISkeleton::GetTooltipText() const
{
    return LOCTEXT( "MRISkeleton_Tooltip", "Drive skeleton based on streamed motion capture data from an MRI server." );
}


FText UAnimGraphNode_MRISkeleton::GetMenuCategory() const
{
    return LOCTEXT( "MRISkeleton_MenuCategory", "MRI" );
}


#undef LOCTEXT_NAMESPACE
