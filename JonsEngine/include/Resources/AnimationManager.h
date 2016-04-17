#pragma once

#include "include/Resources/Animation.h"
#include "include/Resources/AnimationInstance.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Types.h"
#include "include/Core/Utils/Time.h"

#include <vector>

namespace JonsEngine
{
    class AnimationManager
    {
    public:
        AnimationManager();
        ~AnimationManager();

        void Update(const Milliseconds elapsedTime);




    private:
        Mat4& InterpolateTransform(const uint32_t transformIndex, const Milliseconds elapsedTime);

        IDMap<Animation> mAnimations;
        std::vector<AnimationInstance> mActiveAnimations;
    };
}