#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Video/OpenGL/RenderOpenGL.h"
#include "include/Input/InputManager.h"

#include "GL/glfw.h"

namespace JonsEngine
{

    Engine::Engine() : mRunning(false), mLog(Globals::GetEngineLogger()), mMemoryAllocator(Globals::GetDefaultHeapAllocator()), mRenderBackend(NULL), mInputManager(NULL)
    {

    }

    Engine::~Engine()
    {
        if (mRunning)
            Destroy();
    }

    bool Engine::Init()
    {
        if (!mRunning)
        {
            bool res = true;

            JONS_LOG_INFO(mLog, "-------- INITIALIZING ENGINE --------")

            // intialize subsystems
            res &= InitializeModules();

            if (res && !mRunning)
            {
                mRunning = true;
                JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
            }

            return res;
        }
        else
        {
            JONS_LOG_WARNING(mLog, "Engine::Init(): Engine already initialized!")

            return false;
        }
    }

    bool Engine::Destroy()
    {
        if (mRunning)
        {
            bool res = true;

            JONS_LOG_INFO(mLog, "-------- SHUTTING DOWN ENGINE --------")

            res &= DestroyModules();

            if (res && mRunning)
            {
                mRunning = false;
                JONS_LOG_INFO(mLog, "-------- ENGINE DESTROYED --------")
            }

            return res;
        }
        else
        {
            JONS_LOG_WARNING(mLog, "Engine::Destroy(): Engine not initialized!")

            return false;
        }
    }

    bool Engine::isRunning()
    {
        return mRunning;
    }

    void Engine::Tick()
    {
        if (mRunning)
        {
            mRenderBackend->StartFrame();

            mRenderBackend->RenderVertexArrays();

            mRenderBackend->EndFrame();
        }
    }

    bool Engine::InitializeModules()
    {
        bool initResult = true;

        // Create and Initialize rendering backend
        if (!mRenderBackend)
        {
            if (mEngineSettings.RenderBackend == OPENGL)
                mRenderBackend = mMemoryAllocator.AllocateObject<RenderOpenGL>();

            if (!mRenderBackend->Init(mEngineSettings))
            {
                JONS_LOG_ERROR(mLog, "Engine::InitializeModules(): Renderer failed to initialize!")
                return false;
            }
        }

        if (!mInputManager)
        {
            mInputManager = mMemoryAllocator.AllocateObject<InputManager>();
            if (!mInputManager->Init(mEngineSettings))
            {
                JONS_LOG_ERROR(mLog, "Engine::InitializeModules(): InputManager failed to initialize!")
                return false;
            }
        }

        return (mRenderBackend != NULL && mInputManager != NULL);
    }

    bool Engine::DestroyModules()
    {
        // Destroy rendering backend
        if (mRenderBackend)
        {
            mMemoryAllocator.DeallocateObject<RenderBase>(mRenderBackend);
            mRenderBackend = NULL;
        }

        // Destroy input manager backend
        if (mInputManager)
        {
            mMemoryAllocator.DeallocateObject<InputManager>(mInputManager);
            mInputManager = NULL;
        }

        return (!mRenderBackend && !mInputManager);
    }
}