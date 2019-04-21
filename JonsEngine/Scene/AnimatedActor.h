#pragma once

#include "Core/Containers/IDMap.hpp"
#include "Core/Utils/TimeUtils.h"
#include "Resources/Animation.h"
#include "Resources/AnimationInstance.h"
#include "Scene/BaseActor.h"

#include <functional>

namespace JonsEngine
{
	class Scene;

    class AnimatedActor : public BaseActor
    {
    public:
        typedef std::function<AnimationInstanceID(const ModelID, const AnimationID, const bool)> OnPlayAnimationFunc;
		typedef std::function<void(const AnimationInstanceID)> OnRestartAnimationFunc;
        typedef std::function<void(AnimationInstanceID&)> OnStopAnimationFunc;

        AnimatedActor( const OnPlayAnimationFunc& onPlayAnimation, const OnRestartAnimationFunc& onRestartAnimation, const OnStopAnimationFunc& onStopAnimation,
			const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID);
        ~AnimatedActor();

        void PlayAnimation(const AnimationID animationID, const Milliseconds animationTime, const bool repeat);
		void RestartAnimation();
        void StopAnimation();

        AnimationID GetActiveAnimation() const;
        bool IsRepeatingAnimation() const;
        bool IsPlaying() const;
        Milliseconds ElapstedAnimationTime() const;

        void UpdateTimestamp(const Milliseconds elapsedTime);
        AnimationInstanceID GetAnimationInstance() const;

	private:
		friend class Scene;
		void SetCallbacks( const OnPlayAnimationFunc& onPlayAnimation, const OnRestartAnimationFunc& onRestartAnimation, const OnStopAnimationFunc& onStopAnimation );

    private:
        OnPlayAnimationFunc mOnPlayAnimationFunc;
		OnRestartAnimationFunc mOnRestartAnimationFunc;
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