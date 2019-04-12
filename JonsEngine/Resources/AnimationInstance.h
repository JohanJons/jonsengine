#pragma once

#include "Model.h"
#include "Animation.h"
#include "Bone.h"
#include "Time.h"
#include "Types.h"
#include "IDGenerator.hpp"

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