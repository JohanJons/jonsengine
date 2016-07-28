#include "include/Resources/Animation.h"

namespace JonsEngine
{
    Animation::Animation(const std::string& name, const Milliseconds duration, const BoneTransforms& boneTransforms, const Mat4& inverseRootMatrix, const ParentMap& parentMap) :
        mName(name),
        mAnimationDuration(duration),
		mNumBones(boneTransforms.size()),
        mInverseRootMatrix(inverseRootMatrix),
		mParentMap(parentMap)
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
		return mNumBones;
	}

    const Mat4& Animation::GetInverseRootMatrix() const
    {
        return mInverseRootMatrix;
    }

	const BoneIndex Animation::GetParentIndex(const BoneIndex bone) const
    {
        return mParentMap.at(bone);
    }

	const BoneTransforms& Animation::GetBindPoseTransforms() const
	{
		return mBindPoseTransforms;
	}
}