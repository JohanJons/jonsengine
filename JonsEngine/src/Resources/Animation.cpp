#include "include/Resources/Animation.h"

namespace JonsEngine
{
    Animation::Animation(const std::string& name, const Milliseconds duration, const Mat4& inverseRootMatrix, const BoneAnimationContainer& boneAnimations, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets) :
        mName(name),
        mAnimationDuration(duration),
        mInverseRootMatrix(inverseRootMatrix),
		mParentMap(parentMap),
		mBoneOffsetTransforms(boneOffsets),
		mBoneAnimations(boneAnimations)
    {
    }

	Animation::Animation(const Animation& other, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets) :
		mName(other.mName),
		mAnimationDuration(other.mAnimationDuration),
		mInverseRootMatrix(other.mInverseRootMatrix),
		mParentMap(parentMap),
		mBoneOffsetTransforms(boneOffsets),
		mBoneAnimations(other.mBoneAnimations)
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


	const Mat4& Animation::GetInverseRootMatrix() const
	{
		return mInverseRootMatrix;
	}

	const Mat4& Animation::GetBoneOffsetTransform(const BoneIndex bone) const
	{
		return mBoneOffsetTransforms.at(bone);
	}

	const BoneIndex Animation::GetParentIndex(const BoneIndex bone) const
	{
		return mParentMap.at(bone);
	}

	Mat4 Animation::InterpolateBoneTransform(const BoneIndex bone, const Milliseconds elapsedTime) const
	{
		const BoneAnimation& boneAnimation = mBoneAnimations.at(bone);

		return gIdentityMatrix;
	}
}