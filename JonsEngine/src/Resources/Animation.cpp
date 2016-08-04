#include "include/Resources/Animation.h"

namespace JonsEngine
{
    Animation::Animation(const std::string& name, const Milliseconds duration, const Mat4& inverseRootMatrix, const BoneAnimationContainer& boneAnimations, const ConstRangedIterator<BoneParentMap>& parentIter, ConstRangedIterator<std::vector<PackageBone>> boneIter) :
        mName(name),
        mAnimationDuration(duration),
        mInverseRootMatrix(inverseRootMatrix),
		mBoneAnimations(boneAnimations)
    {
		for (const auto boneIndex : parentIter)
			mParentMap.emplace_back(boneIndex);

		for (const PackageBone& bone : boneIter)
			mBoneOffsetTransforms.emplace_back(bone.mOffsetMatrix);
    }
    
    Animation::~Animation()
    {
    }


    const std::string& Animation::GetName() const
    {
        return mName;
    }

    Milliseconds Animation::GetAnimationDuration() const
    {
        return mAnimationDuration;
    }

	uint32_t Animation::GetNumberOfBones() const
	{
		return mBoneAnimations.size();
	}


	const BoneIndex Animation::GetParentIndex(const BoneIndex bone) const
	{
		return mParentMap.at(bone);
	}

	Mat4 Animation::InterpolateBoneTransform(const Mat4& currentTransform, const BoneIndex bone, const Milliseconds elapsedTime) const
	{
		const BoneAnimation& boneAnimation = mBoneAnimations.at(bone);

		return gIdentityMatrix;
	}
}