#include "Engine.h"

#include "Logger.h"
#include "HeapAllocator.h"
#include "EngineSettings.h"
#include "DebugOptions.h"
#include "Time.h"
#include "Scene.h"
#include "WindowManager.h"
#include "ResourceManifest.h"

#include <exception>
#include <functional>


namespace JonsEngine
{
    Engine::Engine(const EngineSettings& settings) : mLog(Logger::GetCoreLogger()), 
                                                     mMemoryAllocator("EngineHeapAllocator"),

                                                     mWindow(settings, mLog), 
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
		auto windowDimensions = mWindow.GetWindowDimensions();
        const float windowAspectRatio = windowDimensions.x / static_cast<float>(windowDimensions.y);

        // update scene actors
		Scene& activeScene = mSceneManager.GetActiveScene();
        activeScene.Tick(elapstedFrameTime, windowAspectRatio);
		auto dirtyFlags = activeScene.GetAndResetDirtyFlags();

        // parse scene into renderqueue for renderer
        const float zNear = mRenderer.GetZNear(), zFar = mRenderer.GetZFar();
        const RenderQueue& renderQueue = mSceneParser.ParseScene(activeScene, dirtyFlags, debugOptions, windowAspectRatio, zNear, zFar);

        // render the scene
        mRenderer.Render(renderQueue, elapstedFrameTime, debugOptions.mRenderingFlags);

        mLastFrameTime = currentFrameTime;
    }
}