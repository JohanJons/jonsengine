#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/DebugOptions.h"
#include "include/Scene/Scene.h"
#include "include/Window/WindowManager.h"
#include "include/Resources/ResourceManifest.h"

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
                                                     mSceneManager()
    {
        JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
    }

    Engine::~Engine()
    {
        JONS_LOG_INFO(mLog, "-------- DESTROYING ENGINE --------")
    }


    void Engine::Tick(const DebugOptions& debugOptions)
    {
        mWindow.Poll();

        // reset render queue
        mRenderQueue.Clear();

		Scene* activeScene = mSceneManager.GetActiveScene();

        // update model matrix of all nodes in active scene
        // TODO: only on change in scene node
		activeScene->GetRootNode().UpdateModelMatrix(gIdentityMatrix);

        // get renderqueue from scene
        const RenderQueue& renderQueue = activeScene->GetRenderQueue(mWindow.GetScreenWidth(), mWindow.GetScreenHeight(), mRenderer.GetZNear(), mRenderer.GetZFar());

        // render the scene
        mRenderer.Render(mRenderQueue, debugOptions.mRenderingFlags);
    }
}