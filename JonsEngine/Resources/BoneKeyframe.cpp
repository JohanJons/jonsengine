#pragma once

#include "Resources/BoneKeyframe.h"
#include "Resources/JonsPackage.h"

namespace JonsEngine
{
	BoneKeyframe::BoneKeyframe()
	{
	}

	BoneKeyframe::BoneKeyframe(const PackageBoneKeyframe& pkgKeyframe) :
		mTimestamp(pkgKeyframe.mTimestampMillisec),
		mTranslation(pkgKeyframe.mTranslation),
		mRotation(pkgKeyframe.mRotation)
	{
	}

	BoneKeyframe::BoneKeyframe(const Milliseconds timestamp, const Vec3& translation, const Quaternion& rotation) :
		mTimestamp(timestamp),
		mTranslation(translation),
		mRotation(rotation)
	{
	}
}