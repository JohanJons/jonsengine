#include "include/RenderQueue/RenderableSkeleton.h"

namespace JonsEngine
{
    RenderableSkeleton::RenderableSkeleton() : RenderableSkeleton(INVALID_BONE_INDEX, INVALID_BONE_INDEX)
    {
    }

    RenderableSkeleton::RenderableSkeleton(const BoneIndex begin, const BoneIndex end) :
        mBonesBegin(begin),
        mBonesEnd(end)
    {
    }
}