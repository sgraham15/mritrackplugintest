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
#pragma once

#include <Animation/AnimInstance.h>
#include "MRIAnimInstance.generated.h"

class UMRISkeletonComponent;

UCLASS( Transient )
class MRITRACKPLUGIN_API UMRIAnimInstance : public UAnimInstance
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="MRI" )
	FName SourceSkeletonAssetName;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="MRI" )
	class AMRICaptureVolume* CaptureVolume;
};
