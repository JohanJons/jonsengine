#pragma once

#include "include/Core/Types.h"
#include "include/Renderer/RenderSettings.h"

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
		RenderSettings mRenderSettings;

        // window settings
        std::string mWindowTitle;
        uint16_t mWindowWidth;
        uint16_t mWindowHeight;
        bool mFullscreen;
        uint16_t mFrameLimit;

        // scene settings
        enum class CullingStrategy
        {
            STANDARD,
            AGGRESSIVE
        } mSceneCullingStrategy;


        EngineSettings();
    };


	inline EngineSettings::EngineSettings() :
        // window settings
        mWindowTitle("JonsEngine Game"),
        mWindowWidth(1920),
        mWindowHeight(1080),
        mFullscreen(false),
        mFrameLimit(0),

        // scene settings
        mSceneCullingStrategy(CullingStrategy::STANDARD)
    {
    }
}