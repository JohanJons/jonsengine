#pragma once

#include "include/Resources/Bone.h"
#include "include/Resources/BoneKeyframe.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"
#include "include/Core/Utils/Time.h"
#include "include/Core/Utils/IDGenerator.hpp"
#include "include/Core/Types.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Containers/RangedIterator.hpp"

#include <string>
#include <vector>
#include <limits>

namespace JonsEngine
{
	typedef IDGenerator32 AnimationIDGenerator;
	typedef AnimationIDGenerator::ID AnimationID;
	static const AnimationID INVALID_ANIMATION_ID = AnimationIDGenerator::INVALID_ID;

	struct PackageAnimation;

    class Animation
    {
    public:
        static const uint32_t MAX_BONES_PER_VERTEX = NUM_BONES_PER_VERTEX;

        Animation(const PackageAnimation& pkgAnimation, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets);
		Animation(const Animation& other, const BoneParentMap& parentMap, const BoneTransforms& boneOffsets);

        const std::string& GetName() const;
        Milliseconds GetAnimationDuration() const;
		uint32_t GetNumberOfBones() const;

		const Mat4& GetInverseRootMatrix() const;
		const Mat4& GetBoneOffsetTransform(const BoneIndex bone) const;
		const BoneIndex GetParentIndex(const BoneIndex bone) const;
		Mat4 InterpolateBoneTransform(const BoneIndex bone, const Milliseconds currTime, const bool repeatingAnimation) const;


    private:
		typedef KeyframeContainer::const_iterator KeyframeIterator;

		KeyframeIterator GetBoneKeyframe(const KeyframeContainer& keyframeContainer, const Milliseconds time) const;
		KeyframeIterator GetNextFrameIter(const KeyframeContainer& keyframeContainer, const KeyframeIterator currFrameIter) const;
        float GetInterpolationFactor(const KeyframeContainer& keyframeContainer, const KeyframeIterator currIter, const KeyframeIterator nextIter, const Milliseconds currElapsedTime) const;


        std::string mName;
        Milliseconds mAnimationDuration;
        Mat4 mInverseRootMatrix;
		const BoneParentMap& mParentMap;
		const BoneTransforms& mBoneOffsetTransforms;
		BoneAnimationContainer mKeyframes;
    };
}