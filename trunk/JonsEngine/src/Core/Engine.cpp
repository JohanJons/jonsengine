#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Scene/Scene.h"


namespace JonsEngine
{

    Engine::Engine(const EngineSettings& settings) : mLog(Logger::GetCoreLogger()), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mRenderer(settings), mInputManager(settings)
    {
        JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
    }

    Engine::~Engine()
    {
        JONS_LOG_INFO(mLog, "-------- ENGINE DESTROYED --------")
    }

    void Engine::Tick()
    {
        mRenderer.StartFrame();

        mInputManager.Poll();

        //mSceneManager.GetActiveScene().Render(mRenderer);

        mRenderer.RenderVertexArrays();

        mRenderer.EndFrame();
    }
}