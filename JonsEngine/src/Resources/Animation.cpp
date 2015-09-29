#include "include/Resources/Animation.h"

namespace JonsEngine
{
    Animation::Animation() : mName(""), mDurationInSeconds(0.0)
    {
    }

    Animation::~Animation()
    {
    }


    void Animation::Play(const bool doPlay)
    {
        mIsPlaying = doPlay;
    }


    bool Animation::IsPlaying() const
    {
        return mIsPlaying;
    }

    double Animation::GetTotalDurationInSeconds() const
    {
        return mDurationInSeconds;
    }

    const std::string& Animation::GetName() const
    {
        return mName;
    }
}