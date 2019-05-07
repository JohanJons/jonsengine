#pragma once

#include "Resources/Model.h"
#include "Resources/Animation.h"
#include "Resources/Bone.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/Types.h"
#include "Core/Utils/IDGenerator.hpp"

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