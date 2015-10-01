#include "include/Resources/ModelAnimation.h"

namespace JonsEngine
{
    ModelAnimation::ModelAnimation(const PackageAnimation& pkgAnimation) : mName(pkgAnimation.mName), mDurationInSeconds(pkgAnimation.mDurationInSeconds)
    {
    }

    ModelAnimation::~ModelAnimation()
    {
    }


    bool ModelAnimation::IsPlaying() const
    {
        return mIsPlaying;
    }

    double ModelAnimation::GetTotalDurationInSeconds() const
    {
        return mDurationInSeconds;
    }

    const std::string& ModelAnimation::GetName() const
    {
        return mName;
    }
}