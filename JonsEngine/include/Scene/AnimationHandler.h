#pragma once

#include "include/Resources/Animation.h"
#include "include/Resources/AnimationInstance.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Types.h"
#include "include/Core/Utils/Time.h"

#include <vector>

namespace JonsEngine
{
    class ResourceManifest;

    class AnimationHandler
    {
    public:
        AnimationHandler(const ResourceManifest& resourceManifest);
        ~AnimationHandler();

        void Update(const Milliseconds elapsedTime);

        AnimationInstanceID PlayAnimation(const AnimationID animationID);
        void StopAnimation(AnimationInstanceID& animationInstance);
        const AnimationInstance::BoneData& GetBoneData(const AnimationID animationID) const;


    private:
        Mat4 InterpolateTransform(const uint32_t transformIndex, const Milliseconds elapsedTime);


        const ResourceManifest& mResourceManifest;

        IDMap<AnimationInstance> mActiveAnimations;
    };
}