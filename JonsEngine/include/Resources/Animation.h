#pragma once

#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Containers/RangedIterator.hpp"
#include "include/Resources/Bone.h"
#include "include/Resources/BoneAnimation.h"
#include "include/Resources/JonsPackage.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"

#include <string>
#include <vector>
#include <limits>

namespace JonsEngine
{
	// TODO: split animation into skeleton/animation

    typedef uint32_t AnimationID;
    static const AnimationID INVALID_ANIMATION_ID = std::numeric_limits<AnimationID>::max();

    class Animation
    {
    public:
        static const uint32_t MAX_BONES_PER_VERTEX = NUM_BONES_PER_VERTEX;

        Animation(const std::string& name, const Milliseconds duration, const Mat4& inverseRootMatrix, const BoneAnimationContainer& boneAnimations, const ConstRangedIterator<BoneParentMap>& parentIter, ConstRangedIterator<std::vector<PackageBone>> boneIter);
        ~Animation();

        const std::string& GetName() const;
        Milliseconds GetAnimationDuration() const;
		uint32_t GetNumberOfBones() const;

		const Mat4& GetInverseRootMatrix() const;
		const Mat4& GetBoneOffsetTransform(const BoneIndex bone) const;
		const BoneIndex GetParentIndex(const BoneIndex bone) const;
		Mat4 InterpolateBoneTransform(const BoneIndex bone, const Milliseconds elapsedTime) const;


    private:
        std::string mName;
        Milliseconds mAnimationDuration;
        Mat4 mInverseRootMatrix;
		BoneParentMap mParentMap;
		BoneTransforms mBoneOffsetTransforms;
		BoneAnimationContainer mBoneAnimations;
    };
}