#include "include/Resources/Animation.h"

namespace JonsEngine
{
    Animation::Animation(const std::string& name, const Milliseconds duration, const uint32_t numBones, const Mat4& inverseRootMatrix) :
        mName(name),
        mAnimationDuration(duration),
		mNumBones(numBones),
        mInverseRootMatrix(inverseRootMatrix)
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

    const Animation::ParentMap& Animation::GetParentMapping() const
    {
        return mParentMap;
    }
}