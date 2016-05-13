#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    typedef std::vector<Mat4> RenderableBones;

    struct RenderableSkeleton
    {
        typedef RenderableBones::size_type BoneIndex;
        static const BoneIndex INVALID_BONE_INDEX;

        RenderableSkeleton();
        RenderableSkeleton(const BoneIndex begin, const BoneIndex end);


        BoneIndex mBoneIndexBegin;
        BoneIndex mBoneIndexEnd;
    };
}