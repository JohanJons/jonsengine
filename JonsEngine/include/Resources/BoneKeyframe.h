#pragma once

#include "include/Core/Types.h"
#include "include/Core/Utils/Time.h"

#include <vector>

namespace JonsEngine
{
	struct PackageBoneKeyframe;

	struct BoneKeyframe
	{
		BoneKeyframe();
		BoneKeyframe(const PackageBoneKeyframe& pkgKeyframe);
		BoneKeyframe(const Milliseconds timestamp, const Vec3& translation, const Quaternion& rotation);


		Milliseconds mTimestamp;
		Vec3 mTranslation;
		Quaternion mRotation;
	};

	typedef std::vector<BoneKeyframe> KeyframeContainer;
	typedef std::vector<KeyframeContainer> BoneKeyframeContainer;
}