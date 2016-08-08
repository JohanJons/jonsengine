#pragma once

#include "include/Resources/BoneAnimation.h"

namespace JonsEngine
{
	BoneAnimation::BoneAnimation() :
		BoneAnimation(INVALID_BONE_INDEX)
	{
	}

	BoneAnimation::BoneAnimation(const BoneIndex boneIndex) :
		mBoneIndex(boneIndex)
	{
	}


	const BoneKeyframe& BoneAnimation::GetBoneKeyframe(const Milliseconds time) const
	{
		for (const BoneKeyframe& keyframe : mKeyframes)
		{
			if (keyframe.mTimestamp > time)
				return keyframe;
		}

		return mKeyframes.back();
	}
}