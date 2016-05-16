#pragma once

#include "include/RenderQueue/RenderableCollection.h"
#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    struct RenderableBone : RenderableCollection<RenderableBone>
    {
        Mat4 mBoneTransform;
    };
}