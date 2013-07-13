#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Window/IWindow.h"
#include "include/Renderer/IRenderer.h"
#include "include/Input/InputBackend.h"

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
        // Window
        IWindow::WindowBackendType mWindowBackend;
        ScreenMode mScreenMode;
        std::string mWindowTitle;

        // Renderer
        IRenderer::RenderBackendType mRenderBackend;

        // Input
        InputBackend::InputBackendType mInputBackend;


        EngineSettings();
    };


    /* EngineSettings inlines */
    inline EngineSettings::EngineSettings() : mWindowBackend(IWindow::GLFW), mWindowTitle("JonsEngine Game"), mRenderBackend(IRenderer::OPENGL), mInputBackend(InputBackend::GLFW)
    {
    }
}