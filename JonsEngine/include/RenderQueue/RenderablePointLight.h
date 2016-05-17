#pragma once

#include "include/RenderQueue/RenderableCollection.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    struct RenderablePointLight : RenderableCollection
    {
        RenderablePointLight();


        Vec4 mLightColor;
        Vec3 mLightPosition;
        float mLightIntensity;
        float mLightRadius;
    };
}