#pragma once

#include "include/Core/EngineSettings.h"
#include "include/Input/InputManager.h"

namespace JonsEngine
{
    class IMemoryAllocator;
    class Logger;
    class RenderBase;

    /* Engine definition */
    class Engine
    {
    public:
        Engine(const EngineSettings& settings);
        ~Engine();

        void Tick();

        const EngineSettings& GetEngineSettings() const;

        RenderBase* GetRenderer() const;
        InputManager& GetInputManager();


    private:
        bool InitializeModules();
        bool DestroyModules();

        EngineSettings mEngineSettings;
        Logger& mLog;
        IMemoryAllocator& mMemoryAllocator;

        /* Modules */
        RenderBase* mRenderBackend;
        InputManager mInputManager;
    };


    /* Engine inlines */
    inline const EngineSettings& Engine::GetEngineSettings() const      { return mEngineSettings;   }
    inline RenderBase* Engine::GetRenderer() const                      { return mRenderBackend;    }
    inline InputManager& Engine::GetInputManager()                      { return mInputManager;     }

}
