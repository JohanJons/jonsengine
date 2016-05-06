#pragma once

#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"
#include "include/Core/Math/Math.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"

#include <string>
#include <vector>
#include <limits>

namespace JonsEngine
{
    typedef uint32_t AnimationID;
    static const AnimationID INVALID_ANIMATION_ID = std::numeric_limits<AnimationID>::max();

    class Animation
    {
    public:
        typedef uint32_t TransformIndex;
        static const uint32_t MAX_NUM_BONES = 64;
        static const uint32_t MAX_BONES_PER_VERTEX = NUM_BONES_PER_VERTEX;
        typedef std::array<TransformIndex, MAX_NUM_BONES> ParentMap;

        Animation(const std::string& name, const Milliseconds duration, const Mat4& inverseRootMatrix);
        ~Animation();

        const std::string& GetName() const;
        Milliseconds GetAnimationDuration() const;
        const Mat4& GetInverseRootMatrix() const;
        const ParentMap& GetParentMapping() const;


    private:
        std::string mName;
        Milliseconds mAnimationDuration;
        Mat4 mInverseRootMatrix;
        ParentMap mParentMap;
        // TODO: array?
        std::vector<Mat4> mTransforms;
    };
}