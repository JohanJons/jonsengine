#pragma once

#include "include/RenderQueue/RenderableBone.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableSkeleton
    {
        typedef RenderableBone::Index BoneIndex;

        RenderableSkeleton();
        RenderableSkeleton(const BoneIndex begin, const BoneIndex end);


        BoneIndex mBonesBegin;
        BoneIndex mBonesEnd;
    };
}