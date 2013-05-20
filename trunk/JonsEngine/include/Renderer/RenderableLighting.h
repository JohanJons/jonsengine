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
            Vec4 mDiffuseColor;
            Vec4 mAmbientColor;
            Vec4 mSpecularColor;

            Vec4 mLightPosition;
            Vec4 mLightDirection;
            uint32_t mLightType;

            float mConstantAttenutation;
            float mLinearAttenuation;
            float mQuadraticAttenuation;
            float mMaxDistance;

            float padding[3];
        };

        /* Lights definition */
        typedef std::array<Light, 8> Lights;


        Vec4 mGamma;
        Vec3 mViewDirection;
        uint32_t mUsedLights;
       
        Lights mLights;
    };
}