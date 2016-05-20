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

    RenderableDirectionalLight::RenderableDirectionalLight(const Vec4& color, const Vec3& direction, const uint32_t numCascades) :
        mColor(color),
        mDirection(direction),
        mNumCascadesUsed(numCascades)
    {
    }
}