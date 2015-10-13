#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/DebugOptions.h"
#include "include/Core/Utils/Time.h"
#include "include/Scene/Scene.h"
#include "include/Window/WindowManager.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/RenderQueue.h"

#include <exception>
#include <functional>


namespace JonsEngine
{
    Engine::Engine(const EngineSettings& settings) : mLog(Logger::GetCoreLogger()), 
                                                     mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<HeapAllocator>("DefaultHeapAllocator"), 
                                                                      [](HeapAllocator* allocator) { HeapAllocator::GetDefaultHeapAllocator().DeallocateObject(allocator); }),

                                                     mWindow(settings, mMemoryAllocator, mLog), 
                                                     mRenderer(settings, mMemoryAllocator, mLog),
                                                     mResourceManifest(mRenderer, mMemoryAllocator), 
                                                     mSceneManager(mRenderer, mResourceManifest)
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
        const uint32_t windowWidth = mWindow.GetScreenWidth();
        const uint32_t windowHeight = mWindow.GetScreenHeight();

		Scene& activeScene = mSceneManager.GetActiveScene();
        activeScene.Tick(elapstedFrameTime, windowWidth, windowHeight);

        // get renderqueue from scene
        const RenderQueue& renderQueue = activeScene.GetRenderQueue();

        // render the scene
        mRenderer.Render(renderQueue, debugOptions.mRenderingFlags);

        mLastFrameTime = currentFrameTime;
    }
}