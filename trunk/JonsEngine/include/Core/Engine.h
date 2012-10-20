#pragma once

#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
    class IMemoryAllocator;
    class Logger;
    class RenderBase;
    class InputManager;

    /* Engine definition */
    class Engine
    {
    public:
        Engine();
        ~Engine();

        bool Init();
        bool Destroy();
        bool isRunning();
        void Tick();

        EngineSettings& GetEngineSettings();

        RenderBase* GetRenderer() const;
        InputManager* GetInputManager() const;


    private:
        bool InitializeModules();
        bool DestroyModules();

        bool mRunning;
        EngineSettings mEngineSettings;
        Logger& mLog;
        IMemoryAllocator& mMemoryAllocator;

        /* Modules */
        RenderBase* mRenderBackend;
        InputManager* mInputManager;
    };


    /* Engine inlines */
    inline EngineSettings& Engine::GetEngineSettings()         { return mEngineSettings;   }
    inline RenderBase* Engine::GetRenderer() const             { return mRenderBackend;    }
    inline InputManager* Engine::GetInputManager() const       { return mInputManager;     }

}
