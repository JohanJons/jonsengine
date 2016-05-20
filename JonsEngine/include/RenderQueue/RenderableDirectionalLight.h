#pragma once

#include "include/RenderQueue/RenderableCollection.h"
#include "include/Renderer/RenderDefs.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct RenderableDirectionalLight
    {
        struct CascadeSplit : RenderableCollection
        {
            CascadeSplit();


            float mNearZ;
            float mFarZ;
        };

        RenderableDirectionalLight(const Vec4& color, const Vec3& direction, const uint32_t numCascades);
        

        Vec4 mColor;
        Vec3 mDirection;
        uint32_t mNumCascadesUsed;
        std::array<CascadeSplit, gMaxNumShadowmapCascades> mCascades;
    };
}