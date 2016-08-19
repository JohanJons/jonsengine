#include "include/RenderQueue/RenderableSkeleton.h"

namespace JonsEngine
{
    RenderableSkeleton::RenderableSkeleton() :
        RenderableSkeleton(INVALID_BONE_INDEX, INVALID_BONE_INDEX)
    {
    }

    RenderableSkeleton::RenderableSkeleton(const BoneIndex begin, const BoneIndex end) :
        mBoneRange(begin, end)
    {
    }

    RenderableSkeleton::RenderableSkeleton(const BoneIndexRange& boneRange) :
        mBoneRange(boneRange)
    {
    }
}