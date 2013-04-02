#pragma once

#include "include/Core/Types.h"
#include "include/Scene/SceneManager.h"
#include "include/Scene/Light.h"
#include "include/Input/InputManager.h"
#include "include/Renderer/RenderItem.h"

namespace JonsEngine
{
    struct EngineSettings;
    struct Model;
    class IMemoryAllocator;
    class Logger;
    class IWindow;
    class IRenderer;
    class ResourceManifest;
    class SceneNode;

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
        IRenderer& GetRenderer();
        InputManager& GetInputManager();
        ResourceManifest& GetResourceManifest();
        SceneManager& GetSceneManager();


    private:
        IWindow* bootCreateWindow(const EngineSettings& settings);
        IRenderer* bootCreateRenderer(const EngineSettings& engineSettings);
        ResourceManifest* bootCreateResourceManifest();

        void CreateRenderQueue(Scene* scene, std::vector<RenderItem>& renderQueue, const std::vector<LightPtr>& activeLights);
        std::vector<LightPtr> GetActiveLights(const Scene* scene);
        void CreateModelRenderables(const Model* model, const Mat4& viewMatrix, const Mat4& perspectiveMatrix, const Mat4& nodeTransform, std::vector<RenderItem>& renderQueue, const std::vector<LightPtr>& activeLights, const Vec4& ambientLight);
 
        Logger& mLog;
        IMemoryAllocator& mMemoryAllocator;

        /* Modules */
        IWindow* mWindow;
        IRenderer* mRenderer;
        InputManager mInputManager;
        ResourceManifest* mResourceManifest;
        SceneManager mSceneManager;
    };


    /* Engine inlines */
    inline IWindow& Engine::GetWindow()                                 { return *mWindow;           }
    inline IRenderer& Engine::GetRenderer()                             { return *mRenderer;         }
    inline InputManager& Engine::GetInputManager()                      { return mInputManager;      }
    inline ResourceManifest& Engine::GetResourceManifest()              { return *mResourceManifest; }
    inline SceneManager& Engine::GetSceneManager()                      { return mSceneManager;      }
}
