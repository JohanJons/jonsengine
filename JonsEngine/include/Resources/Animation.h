#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Utils/Time.h"
#include "include/Core/Types.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"

#include <vector>

namespace JonsEngine
{
    class Animation
    {
    public:
        typedef uint32_t TransformIndex;
        static const uint32_t MAX_NUM_BONES = 64;
        static const uint32_t MAX_BONES_PER_VERTEX = NUM_BONES_PER_VERTEX;

        Animation();
        ~Animation();




    private:
        std::string mName;
        Milliseconds mAnimationDuration;
        std::array<TransformIndex, MAX_NUM_BONES> mParentMap;
        // TODO: array?
        std::vector<Mat4> mTransforms;
    };

    typedef IDMap<Animation>::ItemID AnimationID;
    static const AnimationID INVALID_ANIMATION_ID = IDMap<Animation>::INVALID_ITEM_ID;
}