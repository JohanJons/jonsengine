#pragma once

#include "include/Core/Types.h"

#include <vector>
#include <limits>
#include <utility>

namespace JonsEngine
{
	typedef Mat4 Bone;
	typedef std::vector<Bone> BoneTransforms;
	typedef BoneTransforms::size_type BoneIndex;
	static constexpr BoneIndex INVALID_BONE_INDEX = std::numeric_limits<BoneIndex>::max();
	typedef std::vector<BoneIndex> BoneParentMap;
	// the range is [begin, end)
	typedef std::pair<BoneIndex, BoneIndex> BoneIndexRange;
}