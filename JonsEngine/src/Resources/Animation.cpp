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
}