#pragma once

#include <string>

#include "include/Core/EngineDefs.h"
#include "include/Video/RenderBackend.h"
#include "include/Input/InputBackend.h"

namespace JonsEngine
{
    /* EngineSettings definition */
    struct EngineSettings
    {
        EngineSettings();

        // Video
        RenderBackend::RenderBackendType RenderBackend;
        ScreenMode ScreenMode;
        std::string WindowTitle;


        // Input
        InputBackend::InputBackendType InputBackend;
    };
}