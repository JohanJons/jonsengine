#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include <array>

namespace JonsEngine
{
    /* RenderableLighting definition */
    struct RenderableLighting
    {
        RenderableLighting(const Vec4& gamma, const Vec3& viewDirection, const uint32_t usedLights) : mGamma(gamma), mViewDirection(viewDirection), mUsedLights(usedLights)
        {
        }

        /* Light definition */
        struct Light
        {
            Light() : mLightColor(1.0f), mLightPosition(0.0f), mLightDirection(0.0f), mLightType(0), mIntensity(1.0f), mRadius(0.0f), mMaxDistance(0.0f)
            {
            }

            Vec4 mLightColor;
            Vec4 mLightPosition;
            Vec4 mLightDirection;

            uint32_t mLightType;
            float mIntensity;
            float mRadius;
            float mMaxDistance;
        };

        /* Lights definition */
        typedef std::array<Light, 8> Lights;


        Vec4 mGamma;
        Vec3 mViewDirection;
        uint32_t mUsedLights;
       
        Lights mLights;
    };
}