#pragma once

#include <string>

#include "include/Core/EngineDefs.h"
#include "include/Window/IWindow.h"
#include "include/Renderer/IRenderer.h"
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
        // Window
        IWindow::WindowBackendType WindowBackend;

        // Video
        IRenderer::RenderBackendType RenderBackend;
        ScreenMode ScreenMode;
        std::string WindowTitle;


        // Input
        InputBackend::InputBackendType InputBackend;


        EngineSettings();
    };


    /* EngineSettings inlines */
    inline EngineSettings::EngineSettings() : WindowBackend(IWindow::GLFW), WindowTitle("JonsEngine Game"), RenderBackend(IRenderer::OPENGL), InputBackend(InputBackend::GLFW)
    {
    }
}