#pragma once

#include "include/Resources/AnimationInstance.h"

namespace JonsEngine
{
    AnimationInstance::AnimationInstance(const AnimationID animationID, const BoneIndex begin, const BoneIndex end) : 
        AnimationInstance(animationID, Milliseconds(0), begin, end)
    {
    }

    AnimationInstance::AnimationInstance(const AnimationID animationID, const Milliseconds timestamp, const BoneIndex begin, const BoneIndex end) :
        mAnimationID(animationID),
        mTimestamp(timestamp),
        mBoneRange(begin, end)
    {
    }
}