#pragma once

#include "include/Core/EngineSettings.h"
#include "include/Video/Renderer.h"
#include "include/Input/InputManager.h"

namespace JonsEngine
{
    class IMemoryAllocator;
    class Logger;

    /*
     * Engine is the central controller object.
     * It creates all subsystems and is responsible for advancing the game.
     */

    /* Engine definition */
    class Engine
    {
    public:
        Engine(const EngineSettings& settings);
        ~Engine();

        void Tick();

        Renderer& GetRenderer();
        InputManager& GetInputManager();


    private:
        Logger& mLog;
        IMemoryAllocator& mMemoryAllocator;

        /* Modules */
        Renderer mRenderer;
        InputManager mInputManager;
    };


    /* Engine inlines */
    inline Renderer& Engine::GetRenderer()                              { return mRenderer;         }
    inline InputManager& Engine::GetInputManager()                      { return mInputManager;     }

}
