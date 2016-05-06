#include "include/Resources/Animation.h"

namespace JonsEngine
{
    Animation::Animation(const std::string& name, const Milliseconds duration, const Mat4& inverseRootMatrix) :
        mName(name),
        mAnimationDuration(duration),
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

    const Mat4& Animation::GetInverseRootMatrix() const
    {
        return mInverseRootMatrix;
    }

    const Animation::ParentMap& Animation::GetParentMapping() const
    {
        return mParentMap;
    }
}