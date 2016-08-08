#pragma once

#include "include/Resources/Bone.h"
#include "include/Resources/BoneKeyframe.h"
#include "include/Core/Utils/Time.h"

#include <vector>

namespace JonsEngine
{
	struct BoneAnimation
	{
		BoneAnimation();
		BoneAnimation(const BoneIndex boneIndex);

		const BoneKeyframe& GetBoneKeyframe(const Milliseconds time) const;


		BoneIndex mBoneIndex;
		BoneKeyframeContainer mKeyframes;
	};

	typedef std::vector<BoneAnimation> BoneAnimationContainer;
}