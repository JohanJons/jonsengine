#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Video/OpenGL/RenderOpenGL.h"
#include "include/Input/InputManager.h"

#include "GL/glfw.h"

namespace JonsEngine
{

    Engine::Engine(const EngineSettings& settings) : mLog(Globals::GetEngineLogger()), mMemoryAllocator(Globals::GetDefaultHeapAllocator()), mEngineSettings(settings), mRenderBackend(NULL), mInputManager(settings)
    {
        JONS_LOG_INFO(mLog, "-------- INITIALIZING ENGINE --------")

        InitializeModules();

        JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
    }

    Engine::~Engine()
    {
        JONS_LOG_INFO(mLog, "-------- SHUTTING DOWN ENGINE --------")

        DestroyModules();

        JONS_LOG_INFO(mLog, "-------- ENGINE DESTROYED --------")
    }

    void Engine::Tick()
    {
        mRenderBackend->StartFrame();

        mInputManager.Poll();

        mRenderBackend->RenderVertexArrays();

        mRenderBackend->EndFrame();
    }

    bool Engine::InitializeModules()
    {
        bool initResult = true;

        // Create and Initialize rendering backend
        if (!mRenderBackend)
        {
            if (mEngineSettings.RenderBackend == EngineSettings::RENDER_OPENGL)
                mRenderBackend = mMemoryAllocator.AllocateObject<RenderOpenGL>(mEngineSettings);
        }

        return (mRenderBackend != NULL);
    }

    bool Engine::DestroyModules()
    {
        // Destroy rendering backend
        if (mRenderBackend)
        {
            mMemoryAllocator.DeallocateObject<RenderBase>(mRenderBackend);
            mRenderBackend = NULL;
        }

        return (!mRenderBackend);
    }
}