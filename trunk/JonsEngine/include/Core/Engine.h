#pragma once

#include "include/Core/Types.h"
#include "include/Scene/SceneManager.h"
#include "include/Scene/Light.h"
#include "include/Renderer/IRenderer.h"
#include "include/Renderer/Renderable.h"
#include "include/Renderer/RenderableLighting.h"

namespace JonsEngine
{
    struct EngineSettings;
    struct Model;
    class IMemoryAllocator;
    class Logger;
    class IWindowManager;
    class IRenderer;
    class ResourceManifest;
    class SceneNode;

    /*
     * Engine is the central controller object.
     * It creates all subsystems and is responsible for advancing the engine state.
     */

    /*
     * TODO:
     * Merge EngineDefs.h types into types.h
     * Fix assimp and ambient/diffuse colors..
     * Check camera upside down?
     * Integrate building with boost
     * Generalize  common scene/resource/.. code
     */

    /* Engine definition */
    class Engine
    {
    public:
        Engine(const EngineSettings& settings);
        ~Engine();

        void Tick();

        IWindowManager& GetWindow();
        RendererRefPtr GetRenderer();
        SceneManager& GetSceneManager();


    private:
        RenderQueue CreateRenderQueue(const Scene* scene);
        RenderableLighting GetLightingInfo(const Scene* scene);
        void CreateModelRenderable(const Model* model, const Mat4& viewMatrix, const Mat4& perspectiveMatrix, const Mat4& nodeTransform, const bool lightingEnabled, RenderQueue& renderQueue);
 
        void OnContextCreated();


        Logger& mLog;
        IMemoryAllocator& mMemoryAllocator;

        /* Modules */
        IWindowManager* mWindow;
        ManagedRenderer mRenderer;
        ResourceManifest* mResourceManifest;
        SceneManager mSceneManager;
    };


    /* Engine inlines */
    inline IWindowManager& Engine::GetWindow()          { return *mWindow;      }
    inline RendererRefPtr Engine::GetRenderer()         { return mRenderer;     }
    inline SceneManager& Engine::GetSceneManager()      { return mSceneManager; }
}
