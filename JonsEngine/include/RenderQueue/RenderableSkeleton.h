#pragma once

#include "include/Resources/Bone.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableSkeleton
    {
        RenderableSkeleton();
        RenderableSkeleton(const BoneIndex begin, const BoneIndex end);
        RenderableSkeleton(const BoneIndexRange& boneRange);


		BoneIndexRange mBoneRange;
    };
}