#pragma once

#include "include/Core/Types.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Scene/SceneManager.h"
#include "include/Scene/Light.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Window/GLFW/GLFWWindowManager.h"
#include "include/Renderer/OpenGL3/OpenGLRenderer.h"
#include "include/Renderer/RenderCommands.h"

namespace JonsEngine
{
    struct EngineSettings;
    struct Model;
    class IMemoryAllocator;
    class Logger;
    class SceneNode;

    /*
     * Engine is the central controller object.
     * It creates all subsystems and is responsible for advancing the engine state.
     */

    /*
     * TODO:
     * replace push_back with emplace_back once boost is compatible with MSVC12
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

        GLFWWindowManager& GetWindow();
        OpenGLRendererPtr GetRenderer();
        SceneManager& GetSceneManager();


    private:
        RenderQueue CreateRenderQueue(const Scene* scene);
        RenderableLighting GetLightingInfo(const Scene* scene);
        void CreateModelRenderable(const Model* model, const Mat4& viewMatrix, const Mat4& perspectiveMatrix, const Mat4& nodeTransform, const bool lightingEnabled, RenderQueue& renderQueue);
 
        void OnContextCreated();


        Logger& mLog;
        HeapAllocator mMemoryAllocator;

        /* Modules */
        GLFWWindowManager mWindow;
        ManagedOpenGLRenderer mRenderer;
        ResourceManifest mResourceManifest;
        SceneManager mSceneManager;
    };


    /* Engine inlines */
    inline GLFWWindowManager& Engine::GetWindow()     { return mWindow;       }
    inline OpenGLRendererPtr Engine::GetRenderer()    { return mRenderer;     }
    inline SceneManager& Engine::GetSceneManager()    { return mSceneManager; }
}
