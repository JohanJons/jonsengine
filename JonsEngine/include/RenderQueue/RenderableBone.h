#pragma once

#include "include/RenderQueue/RenderableContainer.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableBone : RenderableContainer<RenderableBone>
    {
        RenderableBone();


        Mat4 mBoneTransform;
    };
}