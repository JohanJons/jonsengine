#pragma once

#include "include/Core/Types.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Scene/SceneManager.h"
#include "include/Scene/Scene.h"
#include "include/Scene/Model.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Window/WindowManager.h"
#include "include/Renderer/DirectX11/DirectXRenderer.h"
#include "include/Renderer/RenderCommands.h"


namespace JonsEngine
{
    struct EngineSettings;
    struct DebugOptions;
    struct Model;
    class IMemoryAllocator;
    class Logger;
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

        void Tick(const DebugOptions& debugOtions);

        WindowManager& GetWindow();
        DirectXRenderer& GetRenderer();
        SceneManager& GetSceneManager();


    private:
        RenderQueue CreateRenderQueue(const std::vector<ModelPtr>& models, const Mat4& viewProjectionMatrix);
        RenderableLighting GetLightingInfo(const Mat4& projMatrix, const Mat4& viewMatrix, const Mat4& viewProjectionMatrix, const Vec4& ambientLight, const Vec3& cameraPosition, const std::vector<PointLightPtr>& pointLights, const std::vector<DirectionalLightPtr>& directionalLights);
        void CreateModelRenderable(const Model* model, const Mat4& viewProjectionMatrix, const Mat4& nodeTransform, const bool lightingEnabled, RenderQueue& renderQueue);
 

        Logger& mLog;
        IMemoryAllocatorPtr mMemoryAllocator;

        /* Modules */
        WindowManager mWindow;
        DirectXRenderer mRenderer;
        ResourceManifest mResourceManifest;
        SceneManager mSceneManager;
    };


    /* Engine inlines */
    inline WindowManager& Engine::GetWindow()        { return mWindow; }
    inline DirectXRenderer& Engine::GetRenderer()    { return mRenderer;     }
    inline SceneManager& Engine::GetSceneManager()   { return mSceneManager; }
}
