#pragma once

#include <string>

#include "include/Core/EngineDefs.h"
#include "include/Video/RenderBase.h"
#include "include/Input/InputBackend.h"

namespace JonsEngine
{
    /* EngineSettings definition */
    struct EngineSettings
    {
        enum RenderBackendType
        {
            RENDER_OPENGL = 0,
            RENDER_NONE
        };

        EngineSettings();

        // Video
        RenderBackendType RenderBackend;
        ScreenMode ScreenMode;
        std::string WindowTitle;


        // Input
        InputBackend::InputBackendType InputBackend;
    };
}