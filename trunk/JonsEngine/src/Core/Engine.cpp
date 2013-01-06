#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Containers/Vector.h"
#include "include/Core/Utils/Math.h"
#include "include/Scene/Scene.h"
#include "include/Window/GLFW/GLFWWindow.h"
#include "include/Window/DummyWindow.h"
#include "include/Renderer/OpenGL3/OpenGLRenderer.h"
#include "include/Renderer/DummyRenderer.h"

#include "boost/foreach.hpp"


namespace JonsEngine
{

    Engine::Engine(const EngineSettings& settings) : mLog(Logger::GetCoreLogger()), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), 
                                                    mWindow(bootCreateWindow(settings)), mRenderer(bootCreateRenderer(settings)), mInputManager(settings)
    {
        JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
    }

    Engine::~Engine()
    {
        JONS_LOG_INFO(mLog, "-------- DESTROYING ENGINE --------")

        mMemoryAllocator.DeallocateObject(mRenderer);
        mMemoryAllocator.DeallocateObject(mWindow);

        JONS_LOG_INFO(mLog, "-------- ENGINE DESTROYED --------")
    }

    void Engine::Tick()
    {
        mWindow->StartFrame();

        mInputManager.Poll();

        mRenderer->BeginRendering();

        // render the root node and recursively all underlying nodes
        if (mSceneManager.HasActiveScene())
        {
            // create view+perspective matrices for the renderer.
            Scene* activeScene = mSceneManager.GetActiveScene();
            const Mat4 viewMatrix = CreateViewMatrix(activeScene->GetSceneCamera());
            const Mat4 perspectiveMatrix = CreatePerspectiveMatrix(mWindow->GetScreenMode().FOV, mWindow->GetScreenMode().ScreenWidth / (float)mWindow->GetScreenMode().ScreenHeight, 0.5f, 100.0f);
            const Mat4 originalModelMatrix(1.0f);

            RenderSceneNode(&activeScene->GetRootNode(), originalModelMatrix, viewMatrix, perspectiveMatrix);
        }

        mRenderer->EndRendering();

        mWindow->EndFrame();
    }


    IWindow* Engine::bootCreateWindow(const EngineSettings& settings)
    {
        switch (settings.WindowBackend)
        {
            case IWindow::GLFW:
                return mMemoryAllocator.AllocateObject<GLFWWindow>(settings);
                break;

            case IWindow::NONE:
            default:
                return mMemoryAllocator.AllocateObject<DummyWindow>(settings);
                break;
        }
    }

    IRenderer* Engine::bootCreateRenderer(const EngineSettings& engineSettings)
    {
        switch (engineSettings.RenderBackend)
        {
            case IRenderer::OPENGL:
                return mMemoryAllocator.AllocateObject<OpenGLRenderer>(engineSettings);
                break;

            case IRenderer::NONE:
            default:
                return mMemoryAllocator.AllocateObject<DummyRenderer>(engineSettings);
                break;

        }
    }

    void Engine::RenderSceneNode(SceneNode* node, const Mat4& parentModelMatrix, const Mat4& viewMatrix, const Mat4& projectionMatrix)
    {
        const Mat4& modelMatrix = parentModelMatrix * node->GetModelMatrix();

        if (MeshPtr mesh = (node->GetMesh()))
            mRenderer->RenderMesh(mesh, modelMatrix, viewMatrix, projectionMatrix);

        BOOST_FOREACH(SceneNode* childNode, node->GetChildNodes())
            RenderSceneNode(childNode, modelMatrix, viewMatrix, projectionMatrix);
    }
}