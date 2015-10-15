#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Utils/Time.h"
#include "include/Scene/BaseActor.h"

namespace JonsEngine
{
    class AnimatedActor : public BaseActor
    {
    public:
        AnimatedActor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID);
        ~AnimatedActor();

        void PlayAnimation(const bool doPlay);
        void RepeatAnimation(const bool repeatAnimation);
        void SetAnimation(const AnimationID animationId, const Milliseconds animationTime);

        AnimationID GetAnimation() const;
        bool IsRepeatingAnimation() const;
        bool IsPlaying() const;

        void UpdateTimestamp(const Milliseconds elapsedTime);


    private:
        bool mIsAnimating;
        bool mIsRepeating;
        Milliseconds mTimestamp;
        Milliseconds mAnimationTime;
        AnimationID mAnimation;
    };

    typedef IDMap<AnimatedActor>::ItemID AnimatedActorID;
    static const AnimatedActorID INVALID_ANIMATED_ACTOR_ID = IDMapTree<AnimatedActor>::INVALID_ITEM_ID;
}