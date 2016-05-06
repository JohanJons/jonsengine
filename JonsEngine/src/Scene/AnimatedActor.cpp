#include "include/Scene/AnimatedActor.h"

namespace JonsEngine
{
    AnimatedActor::AnimatedActor(const OnPlayAnimationFunc& onPlayAnimation, const OnStopAnimationFunc& onStopAnimation, const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID) :
        BaseActor(name, modelId, sceneNodeID),
        mOnPlayAnimationFunc(onPlayAnimation),
        mOnStopAnimationFunc(onStopAnimation),
        mAnimationData(INVALID_ANIMATION_INSTANCE_ID),
        mIsAnimating(false),
        mIsRepeating(false),
        mTimestamp(0),
        mAnimationDuration(0),
        mAnimation(INVALID_ANIMATION_ID)
    {
    }

    AnimatedActor::~AnimatedActor()
    {
    }


    void AnimatedActor::PlayAnimation(const AnimationID animationID, const Milliseconds animationTime, const bool repeat)
    {
        mAnimation = animationID;
        mIsAnimating = true;
        mIsRepeating = repeat;
        mTimestamp = Milliseconds(0);
        mAnimationDuration = animationTime;

        mAnimationData = mOnPlayAnimationFunc(animationID);
    }

    void AnimatedActor::StopAnimation()
    {
        mIsAnimating = false;
        mIsRepeating = false;

        mOnStopAnimationFunc(mAnimationData);
        mAnimationData = INVALID_ANIMATION_INSTANCE_ID;
    }


    AnimationID AnimatedActor::GetActiveAnimation() const
    {
        return mAnimation;
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