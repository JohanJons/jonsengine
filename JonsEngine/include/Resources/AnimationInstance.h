#pragma once

#include "include/Resources/Animation.h"
#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <array>

namespace JonsEngine
{
    struct AnimationInstance
    {
        AnimationID mAnimationID;
        Milliseconds mTimestamp;
        std::array<Mat4, Animation::MAX_NUM_BONES> mBoneTransforms;


        AnimationInstance(const AnimationID animationID);
    };

    typedef IDMap<AnimationInstance>::ItemID AnimationInstanceID;
    static const AnimationInstanceID INVALID_ANIMATION_INSTANCE_ID = IDMap<AnimationInstance>::INVALID_ITEM_ID;
}