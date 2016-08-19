#pragma once

#include "include/Resources/Model.h"
#include "include/Resources/Animation.h"
#include "include/Resources/Bone.h"
#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"
#include "include/Core/Utils/IDGenerator.hpp"

namespace JonsEngine
{
    struct AnimationInstance
    {
		ModelID mModelID;
        AnimationID mAnimationID;
        Milliseconds mTimestamp;
        BoneIndexRange mBoneRange;
		bool mRepeating;


        AnimationInstance(const ModelID modelID, const AnimationID animationID, const BoneIndex begin, const BoneIndex end, const bool repeating);
    };

	typedef IDGenerator32::ID AnimationInstanceID;
	static constexpr AnimationInstanceID INVALID_ANIMATION_INSTANCE_ID = IDGenerator32::INVALID_ID;
}