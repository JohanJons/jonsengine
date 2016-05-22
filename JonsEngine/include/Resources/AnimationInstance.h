#pragma once

#include "include/Resources/Animation.h"
#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"

#include <array>
#include <limits>

namespace JonsEngine
{
    struct AnimationInstance
    {
        typedef std::array<Mat4, Animation::MAX_NUM_BONES> BoneData;

        AnimationID mAnimationID;
        Milliseconds mTimestamp;
        BoneData mBoneTransforms;


        AnimationInstance(const AnimationID animationID);
    };

	typedef uint32_t AnimationInstanceID;
    static constexpr AnimationInstanceID INVALID_ANIMATION_INSTANCE_ID = std::numeric_limits<AnimationInstanceID>::max();
}