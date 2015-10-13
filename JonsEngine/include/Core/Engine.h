#pragma once

#include "include/Core/Types.h"
#include "include/Core/Memory/IMemoryAllocator.h"
#include "include/Core/Utils/Clock.hpp"
#include "include/Window/WindowManager.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Scene/SceneManager.h"

namespace JonsEngine
{
    struct EngineSettings;
    struct DebugOptions;
    class Logger;


    // Engine is the central controller object.
    // It creates all subsystems and is responsible for advancing the engine state.
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

        WindowManager mWindow;
        DX11Renderer mRenderer;
        ResourceManifest mResourceManifest;
        SceneManager mSceneManager;

        HiResClock mClock;
        HiResClock::TimePoint mLastFrameTime;
        float mPrevMinDepth;
        float mPrevMaxDepth;
    };


    // Engine inlines
    inline WindowManager& Engine::GetWindow()               { return mWindow; }
    inline DX11Renderer& Engine::GetRenderer()              { return mRenderer; }
    inline ResourceManifest& Engine::GetResourceManifest()  { return mResourceManifest; }
    inline SceneManager& Engine::GetSceneManager()          { return mSceneManager; }
}
