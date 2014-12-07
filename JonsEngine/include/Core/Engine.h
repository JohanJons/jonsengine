#pragma once

#include "include/Core/Types.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Scene/SceneManager.h"
#include "include/Scene/Scene.h"
#include "include/Scene/Model.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Window/WindowManager.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
#include "include/Renderer/RenderCommands.h"


namespace JonsEngine
{
    struct EngineSettings;
    struct DebugOptions;
    class Model;
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
        DX11Renderer& GetRenderer();
        ResourceManifest& GetResourceManifest();
        SceneManager& GetSceneManager();


    private:
        void FillRenderQueue(const std::vector<ModelPtr>& allModels, const Mat4& viewProjectionMatrix);
        RenderableLighting GetLightingInfo(const Mat4& projMatrix, const Mat4& viewMatrix, const Mat4& viewProjectionMatrix, const Vec4& ambientLight, const Vec3& cameraPosition, const std::vector<PointLightPtr>& pointLights, const std::vector<DirectionalLightPtr>& directionalLights);
        void CullMeshes(const ModelNode& node, const Mat4& viewProjectionMatrix);
        void AddAllMeshes(const ModelNode& node, const Mat4& viewProjectionMatrix);
        void AddMesh(const Mesh& mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix);

        Logger& mLog;
        IMemoryAllocatorPtr mMemoryAllocator;

        /* Modules */
        WindowManager mWindow;
        DX11Renderer mRenderer;
        ResourceManifest mResourceManifest;
        SceneManager mSceneManager;

        RenderQueue mRenderQueue;
        std::vector<ModelPtr> mVisibleModels;
    };


    /* Engine inlines */
    inline WindowManager& Engine::GetWindow()               { return mWindow; }
    inline DX11Renderer& Engine::GetRenderer()              { return mRenderer; }
    inline ResourceManifest& Engine::GetResourceManifest()  { return mResourceManifest; }
    inline SceneManager& Engine::GetSceneManager()          { return mSceneManager; }
}
