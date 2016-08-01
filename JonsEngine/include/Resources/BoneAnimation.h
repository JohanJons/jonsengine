#pragma once

#include "include/Resources/Bone.h"
#include "include/Resources/BoneKeyframe.h"

#include <vector>

namespace JonsEngine
{
	struct BoneAnimation
	{
		BoneIndex mBoneIndex;
		BoneKeyframeContainer mKeyframes;


		BoneAnimation();
		BoneAnimation(const BoneIndex boneIndex);
	};

	typedef std::vector<BoneAnimation> BoneAnimationContainer;
}