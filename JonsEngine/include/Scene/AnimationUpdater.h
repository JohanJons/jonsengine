#pragma once

#include "include/Resources/Animation.h"
#include "include/Resources/AnimationInstance.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Types.h"
#include "include/Core/Utils/Time.h"

#include <vector>
#include <utility>
#include <unordered_map>

namespace JonsEngine
{
    class ResourceManifest;

    class AnimationUpdater
    {
    public:
		typedef std::vector<Mat4> BoneTransforms;
		typedef BoneTransforms::size_type BoneIndex;
		// [begin, end)
		typedef std::pair<BoneIndex, BoneIndex> BoneIndexRange;
		typedef std::unordered_map<AnimationInstanceID, BoneIndexRange> AnimationInstanceMap;

        AnimationUpdater(const ResourceManifest& resourceManifest);
        ~AnimationUpdater();

        void Update(const Milliseconds elapsedTime);

        AnimationInstanceID PlayAnimation(const AnimationID animationID);
        void StopAnimation(AnimationInstanceID& animationInstance);

        const AnimationInstance::BoneData& GetBoneData(const AnimationInstanceID animationInstance) const;


    private:
        Mat4 InterpolateTransform(const uint32_t transformIndex, const Milliseconds elapsedTime);


        const ResourceManifest& mResourceManifest;

		AnimationInstanceMap mInstanceMap;
		BoneTransforms mBoneTransforms;
    };
}