#include "include/RenderQueue/RenderableDirectionalLight.h"

namespace JonsEngine
{
    RenderableDirectionalLight::CascadeSplit::CascadeSplit() :
        mNearZ(0.0f),
        mFarZ(0.0f),
        mNumStaticMeshes(0),
        mNumAnimatedMeshes(0)
    {
    }

    RenderableDirectionalLight::RenderableDirectionalLight(const Vec4& lightColor, const Vec3& lightDir, const uint32_t numCascades) :
        mLightColor(lightColor),
        mLightDirection(lightDir),
        mNumCascadesUsed(numCascades)
    {
    }
}