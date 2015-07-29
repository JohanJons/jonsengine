#pragma once

#include "include/Core/Types.h"

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
            RESOLUTION_1024,
            RESOLUTION_2048,
            RESOLUTION_4092
        } mShadowResolution;

        enum class ShadowReadbackLatency {
           LATENCY_0,
           LATENCY_1,
           LATENCY_2,
           LATENCY_3
        } mShadowReadbackLatency;

        enum class ShadowFiltering {
            PCF_2X2,
            PCF_3X3,
            PCF_5X5,
            PCF_7X7
        } mShadowFiltering;

        enum class Anisotropic {
            X1,
            X2,
            X4,
            X8,
            X16
        } mAnisotropicFiltering;

        enum class AntiAliasing{
            NONE,
            FXAA
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
        mShadowResolution(ShadowResolution::RESOLUTION_2048),
        mShadowReadbackLatency(ShadowReadbackLatency::LATENCY_2),
        mShadowFiltering(ShadowFiltering::PCF_2X2),
        mAnisotropicFiltering(Anisotropic::X16),
        mAntiAliasing(AntiAliasing::FXAA),
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