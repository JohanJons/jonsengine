#include "include/Scene/AnimatedActor.h"

namespace JonsEngine
{
    AnimatedActor::AnimatedActor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID) :
        BaseActor(name, modelId, sceneNodeID),
        mIsAnimating(false),
        mIsRepeating(false),
        mTimestamp(0.0),
        mAnimation(INVALID_ANIMATION_ID)
    {
    }

    AnimatedActor::~AnimatedActor()
    {
    }


    void AnimatedActor::PlayAnimation(const bool doPlay)
    {
        mIsAnimating = doPlay;
    }

    void AnimatedActor::RepeatAnimation(const bool repeatAnimation)
    {
        mIsRepeating = repeatAnimation;
    }

    void AnimatedActor::SetAnimation(const AnimationID animationId)
    {
        mAnimation = animationId;
    }


    AnimationID AnimatedActor::GetAnimation() const
    {
        return mAnimation;
    }

    bool AnimatedActor::IsRepeatingAnimation() const
    {
        return mIsRepeating;
    }
}