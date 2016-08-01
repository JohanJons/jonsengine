#pragma once

#include "include/Resources/BoneKeyframe.h"

namespace JonsEngine
{
	BoneKeyframe::BoneKeyframe()
	{
	}

	BoneKeyframe::BoneKeyframe(const uint32_t timestampMilliseconds, const Vec3& translation, const Quaternion& rotation) :
		mTimestampMilliseconds(timestampMilliseconds),
		mTranslation(translation),
		mRotation(rotation)
	{
	}
}