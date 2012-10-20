#pragma once

#include <string>

#include "include/Core/EngineDefs.h"
#include "include/Video/RenderBase.h"

namespace JonsEngine
{
    enum RenderBackend
    {
        OPENGL = 0,
        NONE
    };

    /* EngineSettings definition */
    struct EngineSettings
    {
        EngineSettings();

        // Video
        RenderBackend RenderBackend;
        ScreenMode ScreenMode;
        std::string WindowTitle;


        // Input

    };
}