#include "include/RenderQueue/RenderableSkeleton.h"

namespace JonsEngine
{
    RenderableSkeleton::RenderableSkeleton() : RenderableSkeleton(RenderableBone::INVALID_INDEX, RenderableBone::INVALID_INDEX)
    {
    }

    RenderableSkeleton::RenderableSkeleton(const BoneIndex begin, const BoneIndex end) :
        mBonesBegin(begin),
        mBonesEnd(end)
    {
    }
}