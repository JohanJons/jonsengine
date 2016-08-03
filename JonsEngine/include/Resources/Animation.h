#pragma once

#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"
#include "include/Core/Math/Math.h"
#include "include/Resources/Bone.h"
#include "include/Resources/BoneAnimation.h"
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

        Animation(const std::string& name, const Milliseconds duration, const BoneAnimationContainer& boneAnimations, const BoneParentMap& parentMap, const Mat4& inverseRootMatrix);
        ~Animation();

        const std::string& GetName() const;
        Milliseconds GetAnimationDuration() const;
		uint32_t GetNumberOfBones() const;

		const BoneIndex GetParentIndex(const BoneIndex bone) const;
        const Mat4& GetInverseRootMatrix() const;
		Mat4 InterpolateBoneTransform(const Mat4& currentTransform, const BoneIndex bone, const Milliseconds elapsedTime) const;


    private:
        std::string mName;
        Milliseconds mAnimationDuration;
        Mat4 mInverseRootMatrix;
		const BoneParentMap& mParentMap;
		BoneAnimationContainer mBoneAnimations;
    };
}