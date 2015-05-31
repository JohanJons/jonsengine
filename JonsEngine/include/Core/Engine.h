#pragma once

#include "include/Core/Types.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Scene/SceneManager.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Window/WindowManager.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"


namespace JonsEngine
{
    struct EngineSettings;
    struct DebugOptions;
    class IMemoryAllocator;
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

        void Tick(const DebugOptions& debugOtions);

        WindowManager& GetWindow();
        DX11Renderer& GetRenderer();
        ResourceManifest& GetResourceManifest();
        SceneManager& GetSceneManager();


    private:
        Logger& mLog;
        IMemoryAllocatorPtr mMemoryAllocator;

        /* Modules */
        WindowManager mWindow;
        DX11Renderer mRenderer;
        ResourceManifest mResourceManifest;
        SceneManager mSceneManager;
    };


    /* Engine inlines */
    inline WindowManager& Engine::GetWindow()               { return mWindow; }
    inline DX11Renderer& Engine::GetRenderer()              { return mRenderer; }
    inline ResourceManifest& Engine::GetResourceManifest()  { return mResourceManifest; }
    inline SceneManager& Engine::GetSceneManager()          { return mSceneManager; }
}
