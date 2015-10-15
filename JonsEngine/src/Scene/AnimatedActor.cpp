#include "include/Scene/AnimatedActor.h"

namespace JonsEngine
{
    AnimatedActor::AnimatedActor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID) :
        BaseActor(name, modelId, sceneNodeID),
        mIsAnimating(false),
        mIsRepeating(false),
        mTimestamp(0),
        mAnimationTime(0),
        mAnimation(INVALID_ANIMATION_ID)
    {
    }

    AnimatedActor::~AnimatedActor()
    {
    }


    void AnimatedActor::PlayAnimation(const bool doPlay)
    {
        mIsAnimating = doPlay;
        mTimestamp = Milliseconds(0);
    }

    void AnimatedActor::RepeatAnimation(const bool repeatAnimation)
    {
        mIsRepeating = repeatAnimation;
    }

    void AnimatedActor::SetAnimation(const AnimationID animationId, const Milliseconds animationTime)
    {
        mAnimation = animationId;
        mAnimationTime = animationTime;
        mTimestamp = Milliseconds(0);
    }


    AnimationID AnimatedActor::GetAnimation() const
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


    void AnimatedActor::UpdateTimestamp(const Milliseconds elapsedTime)
    {
        assert(IsPlaying());

        mTimestamp += elapsedTime;

        if (!(mTimestamp >= mAnimationTime))
            return;

        if (IsRepeatingAnimation())
            mTimestamp = Milliseconds(0);
        else
            PlayAnimation(false);
    }
}