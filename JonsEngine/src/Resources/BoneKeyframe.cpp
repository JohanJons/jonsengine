#pragma once

#include "include/Resources/BoneKeyframe.h"

namespace JonsEngine
{
	BoneKeyframe::BoneKeyframe()
	{
	}

	BoneKeyframe::BoneKeyframe(const Milliseconds timestamp, const Vec3& translation, const Quaternion& rotation) :
		mTimestamp(timestamp),
		mTranslation(translation),
		mRotation(rotation)
	{
	}
}