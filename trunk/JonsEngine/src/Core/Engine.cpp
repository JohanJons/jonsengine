#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "include/Video/Renderer.h"
#include "include/Input/InputManager.h"


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

        mRenderer.RenderVertexArrays();

        mRenderer.EndFrame();
    }
}