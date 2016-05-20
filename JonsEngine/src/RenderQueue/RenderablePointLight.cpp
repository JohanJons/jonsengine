#include "include/RenderQueue/RenderablePointLight.h"

namespace JonsEngine
{
    RenderablePointLight::RenderablePointLight(const Vec4& color, const Vec3& position, const float intensity, const float radius) :
		mColor(color),
		mPosition(position),
		mIntensity(intensity),
		mRadius(radius)
    {
    }
}