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
        enum ShadowQuality {
            SHADOW_QUALITY_LOW = 0,
            SHADOW_QUALITY_MEDIUM,
            SHADOW_QUALITY_HIGH
        } mShadowQuality;

        enum Anisotropic {
            ANISOTROPIC_1X = 1,
            ANISOTROPIC_2X = 2,
            ANISOTROPIC_4X = 4,
            ANISOTROPIC_8X = 8,
            ANISOTROPIC_16X = 16
        } mAnisotropicFiltering;

        enum MSAA {
            MSAA_1X = 1,
            MSAA_2X = 2,
            MSAA_4X = 4,
            MSAA_8X = 8
        } mMSAA;

        // window settings
        std::string mWindowTitle;
        uint32_t mWindowWidth;
        uint32_t mWindowHeight;
        bool mFullscreen;
        float mFOV;
        uint16_t mFrameLimit;

        EngineSettings();
    };


    /* EngineSettings inlines */
    inline EngineSettings::EngineSettings() : mAnisotropicFiltering(Anisotropic::ANISOTROPIC_16X), mShadowQuality(ShadowQuality::SHADOW_QUALITY_MEDIUM), mMSAA(MSAA::MSAA_4X), mWindowTitle("JonsEngine Game"), mWindowWidth(1920), mWindowHeight(1080), mFullscreen(false), mFOV(70.0f), mFrameLimit(0)
    {
    }
}