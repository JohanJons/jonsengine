#pragma once

#include "Bone.h"
#include "Animation.h"
#include "AnimationInstance.h"
#include "IDMap.hpp"
#include "Types.h"
#include "TimeUtils.h"
#include "IDGenerator.hpp"

#include <vector>
#include <unordered_map>

namespace JonsEngine
{
    class ResourceManifest;

    class AnimationUpdater
    {
    public:
		AnimationUpdater(const ResourceManifest& resourceManifest);
        ~AnimationUpdater();

		// Updates animation bone transforms
		// Note: dosn't remove expired animation instances, must be explicitly removed with StopAnimation()
		// @elapstedTime - time since last Update()
        void Update(const Milliseconds elapsedTime);

        AnimationInstanceID PlayAnimation(const ModelID modelID, const AnimationID animationID, const bool repeating);
		void RestartAnimation(const AnimationInstanceID instanceID);
        void StopAnimation(AnimationInstanceID& animationInstance);

		const BoneTransforms& GetBonedata() const;
		const BoneIndexRange& GetBoneRange(const AnimationInstanceID instanceID) const;


    private:
        typedef std::vector<AnimationInstance> AnimationInstances;
		typedef AnimationInstances::size_type AnimationInstanceIndex;
        typedef std::unordered_map<AnimationInstanceID, AnimationInstanceIndex> AnimationInstanceMap;


        const ResourceManifest& mResourceManifest;
		IDGenerator32 mAnimInstanceIDGen;

        AnimationInstances mActiveAnimations;
        AnimationInstanceMap mAnimationInstanceMap;
		BoneTransforms mBoneTransforms;
    };
}