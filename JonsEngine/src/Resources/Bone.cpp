#include "include/Resources/Bone.h"

namespace JonsEngine
{
	BoneWeight::BoneWeight() :
		mBoneIndices({ INVALID_BONE_INDEX, INVALID_BONE_INDEX, INVALID_BONE_INDEX, INVALID_BONE_INDEX }),
		mBoneWeights({ 0.0f, 0.0f, 0.0f, 0.0f })
	{
	}
}