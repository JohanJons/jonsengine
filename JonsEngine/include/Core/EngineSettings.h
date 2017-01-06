#pragma once

#include "include/Renderer/RenderSettings.h"
#include "include/Window/WindowSettings.h"

namespace JonsEngine
{
    /*
     * EngineSettings is used when bootstraping the engine. 
     * Parameters can be changed to alter the engine properties
     */

    struct EngineSettings : RenderSettings, WindowSettings
    {
        // scene settings
        enum class CullingStrategy
        {
            STANDARD,
            AGGRESSIVE
        } mSceneCullingStrategy;


        EngineSettings();
    };


	inline EngineSettings::EngineSettings() :
        // scene settings
        mSceneCullingStrategy(CullingStrategy::STANDARD)
    {
    }
}