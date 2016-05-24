#pragma once

#include "include/Resources/Animation.h"
#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"
#include "include/Core/Utils/IDGenerator.h"

namespace JonsEngine
{
    struct AnimationInstance
    {
        AnimationID mAnimationID;
        Milliseconds mTimestamp;
        //BoneData mBoneTransforms;


        AnimationInstance(const AnimationID animationID);
    };

	typedef IDGenerator<AnimationInstance>::ID AnimationInstanceID;
	static constexpr AnimationInstanceID INVALID_ANIMATION_INSTANCE_ID = IDGenerator<AnimationInstance>::INVALID_ID;
}