#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include <array>

namespace JonsEngine
{
    /* RenderableLighting definition */
    struct RenderableLighting
    {
        RenderableLighting(const Vec4& ambientLight, const Vec3& viewDirection, const Vec4& gamma) : mLightingInfo(ambientLight, gamma, viewDirection)
        {
        }
        
        /* LightingInfo definition */
        struct LightingInfo
        {
            LightingInfo(const Vec4& ambientLight, const Vec4& gamma, const Vec3& viewDirection) : mAmbientLight(ambientLight), mGamma(gamma), mViewDirection(viewDirection), mUsedLights(0)
            {
            }
            
            Vec4 mAmbientLight;
            Vec4 mGamma;
            Vec3 mViewDirection;
            size_t mUsedLights;
        };

        /* Light definition */
        struct Light
        {
            Light(const Vec4& lightIntensity, const Vec4& lightPosition, const float lightAttenuation, const float maxAttenuation) : mLightIntensity(lightIntensity), mLightPosition(lightPosition), mLightAttenuation(lightAttenuation),
                                                                                                                                     mMaxAttenuation(maxAttenuation)
            {
            }

            Light()
            {
            }

            Vec4 mLightIntensity;
            Vec4 mLightPosition;
            float mLightAttenuation;
            float mMaxAttenuation;
            float padding[2];
        };

        /* Lights definition */
        typedef std::array<Light, 8> Lights;

        LightingInfo mLightingInfo;
        Lights mLights;
    };
}