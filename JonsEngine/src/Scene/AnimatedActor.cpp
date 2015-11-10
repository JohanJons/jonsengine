#include "include/Scene/AnimatedActor.h"

namespace JonsEngine
{
    AnimatedActor::AnimatedActor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID) :
        BaseActor(name, modelId, sceneNodeID),
        mIsAnimating(false),
        mIsRepeating(false),
        mTimestamp(0),
        mAnimationTime(0),
        mAnimationIndex(INVALID_ANIMATION_INDEX)
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

    void AnimatedActor::SetAnimation(const ModelAnimationIndex animationIndex, const Milliseconds animationTime)
    {
        mAnimationIndex = animationIndex;
        mAnimationTime = animationTime;
        mTimestamp = Milliseconds(0);
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


    void AnimatedActor::UpdateTimestamp(const Milliseconds elapsedTime)
    {
        assert(IsPlaying());

        mTimestamp += elapsedTime;

        if (mTimestamp < mAnimationTime)
            return;

        if (IsRepeatingAnimation())
            mTimestamp = Milliseconds(0);
        else
            PlayAnimation(false);
    }
}