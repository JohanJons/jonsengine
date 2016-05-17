#pragma once

#include "include/RenderQueue/RenderableCollection.h"
#include "include/Renderer/RenderDefs.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct RenderableDirectionalLight : RenderableCollection
    {
        struct CascadeSplit
        {
            CascadeSplit();


            float mNearZ;
            float mFarZ;
            uint32_t mNumStaticMeshes;
            uint32_t mNumAnimatedMeshes;
        };

        RenderableDirectionalLight(const Vec4& lightColor, const Vec3& lightDir, const uint32_t numCascades);
        

        Vec4 mLightColor;
        Vec3 mLightDirection;
        uint32_t mNumCascadesUsed;
        std::array<CascadeSplit, gMaxNumShadowmapCascades> mCascades;
    };
}