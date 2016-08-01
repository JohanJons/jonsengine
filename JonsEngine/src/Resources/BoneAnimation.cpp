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
}