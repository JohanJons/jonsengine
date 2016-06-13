#pragma once

#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"
#include "include/Core/Math/Math.h"
#include "include/Resources/Bone.h"
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
        static const uint32_t MAX_NUM_BONES = 64;
        static const uint32_t MAX_BONES_PER_VERTEX = NUM_BONES_PER_VERTEX;
        typedef std::array<BoneIndex, MAX_NUM_BONES> ParentMap;

        Animation(const std::string& name, const Milliseconds duration, const uint32_t numBones, const Mat4& inverseRootMatrix);
        ~Animation();

        const std::string& GetName() const;
        Milliseconds GetAnimationDuration() const;
		uint32_t GetNumberOfBones() const;

        const Mat4& GetInverseRootMatrix() const;
        const BoneIndex GetParentIndex(const BoneIndex bone) const;
		const BoneTransforms& GetBindPoseTransforms() const;


    private:
        std::string mName;
        Milliseconds mAnimationDuration;
		uint32_t mNumBones;
        Mat4 mInverseRootMatrix;
        ParentMap mParentMap;
		BoneTransforms mBindPoseTransforms;
    };
}