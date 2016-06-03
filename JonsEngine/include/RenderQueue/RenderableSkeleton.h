#pragma once

#include "include/Resources/Bone.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableSkeleton
    {
        RenderableSkeleton();
        RenderableSkeleton(const BoneIndexRange& boneRange);


		BoneIndexRange mBoneRange;
    };
}