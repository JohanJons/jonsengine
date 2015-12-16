#include "include/Scene/AnimatedActor.h"

namespace JonsEngine
{
    AnimatedActor::AnimatedActor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID) :
        BaseActor(name, modelId, sceneNodeID),
        mIsAnimating(false),
        mIsRepeating(false),
        mTimestamp(0),
        mAnimationDuration(0),
        mAnimationIndex(INVALID_ANIMATION_INDEX)
    {
    }

    AnimatedActor::~AnimatedActor()
    {
    }


    void AnimatedActor::PlayAnimation(const ModelAnimationIndex animationIndex, const Milliseconds animationTime, const bool repeat)
    {
        mAnimationIndex = animationIndex;
        mIsAnimating = true;
        mIsRepeating = repeat;
        mTimestamp = Milliseconds(0);
        mAnimationDuration = animationTime;
    }

    void AnimatedActor::StopAnimation()
    {
        mIsAnimating = false;
        mIsRepeating = false;
    }


    ModelAnimationIndex AnimatedActor::GetAnimation() const
    {
        return mAnimationIndex;
    }

    bool AnimatedActor::IsRepeatingAnimation() const
    {
        return mIsRepeating;
    }

    bool AnimatedActor::IsPlaying() const
    {
        return mIsAnimating;
    }

    Milliseconds AnimatedActor::ElapstedAnimationTime() const
    {
        return mTimestamp;
    }


    void AnimatedActor::UpdateTimestamp(const Milliseconds elapsedTime)
    {
        assert(IsPlaying());

        mTimestamp += elapsedTime;

        if (mTimestamp < mAnimationDuration)
            return;

        if (IsRepeatingAnimation())
            mTimestamp = Milliseconds(0);
        else
            StopAnimation();
    }
}