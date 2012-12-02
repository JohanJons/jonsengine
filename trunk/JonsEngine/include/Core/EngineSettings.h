#pragma once

#include <string>

#include "include/Core/EngineDefs.h"
#include "include/Video/RenderBackend.h"
#include "include/Input/InputBackend.h"

namespace JonsEngine
{
    /*
     * EngineSettings is used when bootstraping the engine. 
     * Parameters can be changed to alter the engine properties
     */

    /* EngineSettings definition */
    struct EngineSettings
    {
        // Video
        RenderBackend::RenderBackendType RenderBackend;
        ScreenMode ScreenMode;
        std::string WindowTitle;


        // Input
        InputBackend::InputBackendType InputBackend;


        EngineSettings();
    };


    /* EngineSettings inlines */
    inline EngineSettings::EngineSettings() : WindowTitle("JonsEngine Game"), RenderBackend(RenderBackend::OPENGL), InputBackend(InputBackend::GLFW)
    {
    }
}