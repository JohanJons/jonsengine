#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Utils/Time.h"
#include "include/Resources/Animation.h"
#include "include/Resources/AnimationInstance.h"
#include "include/Scene/BaseActor.h"

#include <functional>

namespace JonsEngine
{
    class AnimatedActor : public BaseActor
    {
    public:
        typedef std::function<AnimationInstanceID(const AnimationID)> OnPlayAnimationFunc;
        typedef std::function<void(AnimationInstanceID&)> OnStopAnimationFunc;

        AnimatedActor(const OnPlayAnimationFunc& onPlayAnimation, const OnStopAnimationFunc& onStopAnimation, const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID);
        ~AnimatedActor();

        void PlayAnimation(const AnimationID animationID, const Milliseconds animationTime, const bool repeat);
        void StopAnimation();

        AnimationID GetActiveAnimation() const;
        bool IsRepeatingAnimation() const;
        bool IsPlaying() const;
        Milliseconds ElapstedAnimationTime() const;

        void UpdateTimestamp(const Milliseconds elapsedTime);


    private:
        OnPlayAnimationFunc mOnPlayAnimationFunc;
        OnStopAnimationFunc mOnStopAnimationFunc;
        AnimationInstanceID mAnimationData;

        bool mIsAnimating;
        bool mIsRepeating;
        Milliseconds mTimestamp;
        Milliseconds mAnimationDuration;
        AnimationID mAnimation;
    };

    typedef IDMap<AnimatedActor>::ItemID AnimatedActorID;
    static const AnimatedActorID INVALID_ANIMATED_ACTOR_ID = IDMap<AnimatedActor>::INVALID_ITEM_ID;
}