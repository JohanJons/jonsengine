#pragma once

#include "include/Core/Types.h"
#include "include/Scene/SceneManager.h"
#include "include/Scene/Light.h"
#include "include/Input/InputManager.h"
#include "include/Renderer/Renderable.h"
#include "include/Renderer/RenderableLighting.h"

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

    /*
     * TODO:
     * Merge EngineDefs.h types into types.h
     * Fix assimp and ambient/diffuse colors..
     * Add http://imdoingitwrong.wordpress.com/tag/glsl/ lighting attenuation
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

        IWindow& GetWindow();
        IRenderer& GetRenderer();
        InputManager& GetInputManager();
        SceneManager& GetSceneManager();


    private:
        IWindow* bootCreateWindow(const EngineSettings& settings);
        IRenderer* bootCreateRenderer(const EngineSettings& engineSettings);
        ResourceManifest* bootCreateResourceManifest();

        RenderQueue CreateRenderQueue(const Scene* scene);
        RenderableLighting GetLightingInfo(const Scene* scene);

        void CreateModelRenderable(const Model* model, const Mat4& viewMatrix, const Mat4& perspectiveMatrix, const Mat4& nodeTransform, const bool lightingEnabled, RenderQueue& renderQueue);
 
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
    inline SceneManager& Engine::GetSceneManager()                      { return mSceneManager;      }
}
