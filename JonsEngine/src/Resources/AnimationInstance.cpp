#pragma once

#include "include/Resources/AnimationInstance.h"

namespace JonsEngine
{
    AnimationInstance::AnimationInstance(const ModelID modelID, const AnimationID animationID, const BoneIndex begin, const BoneIndex end, const bool repeating) :
		mModelID(modelID),
		mAnimationID(animationID),
		mTimestamp(0),
		mBoneRange(begin, end),
		mRepeating(repeating)
    {
    }
}