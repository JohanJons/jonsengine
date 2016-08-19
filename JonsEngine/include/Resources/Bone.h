#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"

#include <vector>
#include <array>
#include <limits>
#include <utility>

namespace JonsEngine
{
	typedef Mat4 Bone;
	typedef std::vector<Bone> BoneTransforms;
	typedef uint8_t BoneIndex;
	static constexpr BoneIndex INVALID_BONE_INDEX = std::numeric_limits<BoneIndex>::max();
	static constexpr BoneIndex MAX_NUM_BONES = INVALID_BONE_INDEX - 1;
	static constexpr BoneIndex MAX_BONES_PER_VERTEX = NUM_BONES_PER_VERTEX;
	typedef std::vector<BoneIndex> BoneParentMap;
	// the range is [begin, end)
	typedef std::pair<BoneIndex, BoneIndex> BoneIndexRange;

	struct BoneWeight
	{
		BoneWeight();


		std::array<BoneIndex, NUM_BONES_PER_VERTEX> mBoneIndices;
		std::array<float, NUM_BONES_PER_VERTEX> mBoneWeights;
	};
}