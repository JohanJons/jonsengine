#pragma once

#include "include/RenderQueue/RenderableCollection.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct RenderablePointLight : RenderableCollection
    {
        RenderablePointLight(const Vec4& color, const Vec3& position, const float intensity, const float radius);


        Vec4 mColor;
        Vec3 mPosition;
        float mIntensity;
        float mRadius;
    };
}