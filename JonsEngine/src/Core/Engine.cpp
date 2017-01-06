#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/DebugOptions.h"
#include "include/Core/Utils/Time.h"
#include "include/Scene/Scene.h"
#include "include/Window/WindowManager.h"
#include "include/Resources/ResourceManifest.h"

#include <exception>
#include <functional>


namespace JonsEngine
{
    Engine::Engine(const EngineSettings& settings) : mLog(Logger::GetCoreLogger()), 
                                                     mMemoryAllocator("EngineHeapAllocator"),

                                                     mWindow(settings, mMemoryAllocator, mLog), 
                                                     mRenderer(settings, settings, mMemoryAllocator, mLog),
                                                     mResourceManifest(mRenderer, mMemoryAllocator), 
                                                     mSceneManager(mRenderer, mResourceManifest),
                                                     mSceneParser(settings, mResourceManifest)
    {
        JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
    }

    Engine::~Engine()
    {
        JONS_LOG_INFO(mLog, "-------- DESTROYING ENGINE --------")
    }

    void Engine::Tick(const DebugOptions& debugOptions)
    {
        const HiResClock::TimePoint currentFrameTime = mClock.Now();
        const Milliseconds elapstedFrameTime = mClock.ElapsedTime<Milliseconds>(mLastFrameTime, currentFrameTime);

        // process input and window events
        mWindow.Poll();
        const float windowAspectRatio = mWindow.GetScreenWidth() / static_cast<float>(mWindow.GetScreenHeight());

        // update scene actors
		Scene& activeScene = mSceneManager.GetActiveScene();
        activeScene.Tick(elapstedFrameTime, windowAspectRatio);

        // parse scene into renderqueue for renderer
        const float zNear = mRenderer.GetZNear(), zFar = mRenderer.GetZFar();
        const RenderQueue& renderQueue = mSceneParser.ParseScene(activeScene, debugOptions, windowAspectRatio, zNear, zFar);

        // render the scene
        mRenderer.Render(renderQueue, elapstedFrameTime, debugOptions.mRenderingFlags);

        mLastFrameTime = currentFrameTime;
    }
}