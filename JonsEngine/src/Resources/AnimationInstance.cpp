#pragma once

#include "include/Resources/AnimationInstance.h"

namespace JonsEngine
{
    AnimationInstance::AnimationInstance(const ModelID modelID, const AnimationID animationID, const BoneIndex begin, const BoneIndex end) :
        AnimationInstance(modelID, animationID, Milliseconds(0), begin, end)
    {
    }

    AnimationInstance::AnimationInstance(const ModelID modelID, const AnimationID animationID, const Milliseconds timestamp, const BoneIndex begin, const BoneIndex end) :
		mModelID(modelID),
        mAnimationID(animationID),
        mTimestamp(timestamp),
        mBoneRange(begin, end)
    {
    }
}