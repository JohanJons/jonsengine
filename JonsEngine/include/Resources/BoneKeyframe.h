#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
	struct BoneKeyframe
	{
		uint32_t mTimestampMilliseconds;
		Vec3 mTranslation;
		Quaternion mRotation;


		BoneKeyframe();
		BoneKeyframe(const uint32_t timestampMilliseconds, const Vec3& translation, const Quaternion& rotation);
	};

	typedef std::vector<BoneKeyframe> BoneKeyframeContainer;
}