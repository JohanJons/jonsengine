#pragma once

#include "include/Resources/AnimationInstance.h"

namespace JonsEngine
{
    AnimationInstance::AnimationInstance(const AnimationID animationID) :
        mAnimationID(animationID),
        mTimestamp(0)
    {
    }
}