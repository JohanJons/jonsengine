#pragma once

#include "include/Resources/Animation.h"
#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"

#include <array>

namespace JonsEngine
{
    struct AnimationInstance
    {
        AnimationID mAnimationID;
        Milliseconds mTimestamp;
        std::array<Animation::TransformIndex, Animation::MAX_NUM_BONES>& mParentMap;
        std::array<Mat4, Animation::MAX_NUM_BONES> mBoneTransforms;
    };
}