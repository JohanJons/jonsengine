#pragma once

#include "include/Core/EngineSettings.h"
#include "include/Scene/SceneManager.h"
#include "include/Input/InputManager.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Core/Types.h"
#include "include/Renderer/RenderItem.h"

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
        ResourceManifest& GetResourceManifest();


    private:
        IWindow* bootCreateWindow(const EngineSettings& settings);
        IRenderer* bootCreateRenderer(const EngineSettings& engineSettings);
        ResourceManifest* bootCreateResourceManifest();

        void CreateRenderQueue(Scene* scene, std::vector<RenderItem>& renderQueue);
        void CreateModelRenderables(const Model* model, const Mat4& viewMatrix, const Mat4& perspectiveMatrix, const Mat4& nodeTransform, std::vector<RenderItem>& renderQueue);

        Logger& mLog;
        IMemoryAllocator& mMemoryAllocator;

        /* Modules */
        IWindow* mWindow;
        SceneManager mSceneManager;
        IRenderer* mRenderer;
        InputManager mInputManager;
        ResourceManifest* mResourceManifest;
    };


    /* Engine inlines */
    inline IWindow& Engine::GetWindow()                                 { return *mWindow;           }
    inline SceneManager& Engine::GetSceneManager()                      { return mSceneManager;      }
    inline IRenderer& Engine::GetRenderer()                             { return *mRenderer;         }
    inline InputManager& Engine::GetInputManager()                      { return mInputManager;      }
    inline ResourceManifest& Engine::GetResourceManifest()              { return *mResourceManifest; }
}
