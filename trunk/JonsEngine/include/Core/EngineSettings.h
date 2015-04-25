#pragma once

#include "include/Core/EngineDefs.h"

#include <string>

namespace JonsEngine
{
    /*
     * EngineSettings is used when bootstraping the engine. 
     * Parameters can be changed to alter the engine properties
     */

    /* EngineSettings definition */
    struct EngineSettings
    {
        // render settings
        enum class ShadowResolution {
            SHADOW_RESOLUTION_1024 = 0,
            SHADOW_RESOLUTION_2048,
            SHADOW_RESOLUTION_4092
        } mShadowResolution;

        enum class ShadowReadbackLatency {
            SHADOW_READBACK_LATENCY_0 = 0,
            SHADOW_READBACK_LATENCY_1,
            SHADOW_READBACK_LATENCY_2,
            SHADOW_READBACK_LATENCY_3
        } mShadowReadbackLatency;

        enum class Anisotropic {
            ANISOTROPIC_1X = 1,
            ANISOTROPIC_2X = 2,
            ANISOTROPIC_4X = 4,
            ANISOTROPIC_8X = 8,
            ANISOTROPIC_16X = 16
        } mAnisotropicFiltering;

        enum class AntiAliasing {
            ANTIALIASING_NONE = 0,
            ANTIALIASING_FXAA = 1
        } mAntiAliasing;

        bool mSSAOEnabled;

        // window settings
        std::string mWindowTitle;
        uint16_t mWindowWidth;
        uint16_t mWindowHeight;
        bool mFullscreen;
        uint16_t mFrameLimit;

        EngineSettings();
    };


    /* EngineSettings inlines */
    inline EngineSettings::EngineSettings() :
        // render settings
        mShadowResolution(ShadowResolution::SHADOW_RESOLUTION_2048),
        mShadowReadbackLatency(ShadowReadbackLatency::SHADOW_READBACK_LATENCY_1),
        mAnisotropicFiltering(Anisotropic::ANISOTROPIC_16X),
        mAntiAliasing(AntiAliasing::ANTIALIASING_FXAA),
        mSSAOEnabled(true),

        // window settings
        mWindowTitle("JonsEngine Game"),
        mWindowWidth(1920),
        mWindowHeight(1080),
        mFullscreen(false),
        mFrameLimit(0)
    {
    }
}