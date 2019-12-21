#pragma once

#include "Core/Types.h"
#include "Core/Memory/HeapAllocator.h"
#include "Core/Utils/Clock.hpp"
#include "Window/WindowManager.h"
#include "Renderer/DirectX11/DX11Renderer.h"
#include "Resources/ResourceManifest.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneParser.h"

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

        void Tick(const DebugOptions& debugOptions);

        WindowManager& GetWindow();
        DX11Renderer& GetRenderer();
        ResourceManifest& GetResourceManifest();
        SceneManager& GetSceneManager();


    private:
        Logger& mLog;
        HeapAllocator mMemoryAllocator;

        WindowManager mWindow;
        DX11Renderer mRenderer;
        ResourceManifest mResourceManifest;
        SceneManager mSceneManager;
        SceneParser mSceneParser;

        HiResClock mClock;
        HiResClock::TimePoint mLastFrameTime;
    };
}
