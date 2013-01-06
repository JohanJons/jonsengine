#pragma once

#include "include/Core/EngineSettings.h"
#include "include/Scene/SceneManager.h"
#include "include/Input/InputManager.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    class IWindow;
    class IRenderer;
    class IMemoryAllocator;
    class SceneNode;
    class Logger;

    /*
     * Engine is the central controller object.
     * It creates all subsystems and is responsible for advancing the engine state.
     */

    /* Engine definition */
    class Engine
    {
    public:
        Engine(const EngineSettings& settings);
        ~Engine();

        void Tick();

        IWindow& GetWindow();
        SceneManager& GetSceneManager();
        IRenderer& GetRenderer();
        InputManager& GetInputManager();


    private:
        IWindow* bootCreateWindow(const EngineSettings& settings);
        IRenderer* bootCreateRenderer(const EngineSettings& engineSettings);
        void RenderSceneNode(SceneNode* node, const Mat4& parentModelMatrix, const Mat4& viewMatrix, const Mat4& projectionMatrix);

        Logger& mLog;
        IMemoryAllocator& mMemoryAllocator;

        /* Modules */
        IWindow* mWindow;
        SceneManager mSceneManager;
        IRenderer* mRenderer;
        InputManager mInputManager;
    };


    /* Engine inlines */
    inline IWindow& Engine::GetWindow()                                 { return *mWindow;          }
    inline SceneManager& Engine::GetSceneManager()                      { return mSceneManager;     }
    inline IRenderer& Engine::GetRenderer()                             { return *mRenderer;        }
    inline InputManager& Engine::GetInputManager()                      { return mInputManager;     }
}
