#include "include/Resources/Animation.h"

namespace JonsEngine
{
    Animation::Animation(const std::string& name, const Milliseconds duration, const BoneAnimationContainer& boneAnimations, const BoneParentMap& parentMap, const Mat4& inverseRootMatrix) :
        mName(name),
        mAnimationDuration(duration),
        mInverseRootMatrix(inverseRootMatrix),
		mParentMap(parentMap),
		mBoneAnimations(boneAnimations)
    {
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

    const Mat4& Animation::GetInverseRootMatrix() const
    {
        return mInverseRootMatrix;
    }

	Mat4 Animation::InterpolateBoneTransform(const Mat4& currentTransform, const BoneIndex bone, const Milliseconds elapsedTime) const
	{
		const BoneAnimation& boneAnimation = mBoneAnimations.at(bone);

		return gIdentityMatrix;
	}
}