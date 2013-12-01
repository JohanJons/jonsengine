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
        float mAnisotropicFiltering;

        // window settings
        std::string mWindowTitle;
        uint32_t mWindowWidth;
        uint32_t mWindowHeight;
        bool mFullscreen;
        float mFOV;
        uint16_t mFrameLimit;
        uint16_t mMSAA;

        EngineSettings();
    };


    /* EngineSettings inlines */
    inline EngineSettings::EngineSettings() : mAnisotropicFiltering(0.0f),mWindowTitle("JonsEngine Game"), mWindowWidth(1920), mWindowHeight(1080), mFullscreen(false), mFOV(70.0f), mFrameLimit(0), mMSAA(0)
    {
    }
}