#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
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
                                                    mWindow(bootCreateWindow(settings)), mRenderer(bootCreateRenderer(settings)), mInputManager(settings),
                                                    mResourceManifest(bootCreateResourceManifest())
    {
        JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
    }

    Engine::~Engine()
    {
        JONS_LOG_INFO(mLog, "-------- DESTROYING ENGINE --------")

        mMemoryAllocator.DeallocateObject(mResourceManifest);
        mMemoryAllocator.DeallocateObject(mRenderer);
        mMemoryAllocator.DeallocateObject(mWindow);

        JONS_LOG_INFO(mLog, "-------- ENGINE DESTROYED --------")
    }

    void Engine::Tick()
    {
        mWindow->StartFrame();

        mInputManager.Poll();

        if (mSceneManager.HasActiveScene())
        {
            Scene* activeScene = mSceneManager.GetActiveScene();
            
            // update model matrix of all nodes in active scene
            activeScene->GetRootNode().UpdateModelMatrix(Mat4(1.0f));

            // create the rendering queue and active lights
            std::vector<LightPtr> activeLights;
            std::vector<RenderItem> renderQueue;
            GetActiveLights(activeScene, activeLights);
            CreateRenderQueue(activeScene, renderQueue, activeLights);

            mRenderer->BeginRendering();

            // render the scene
            mRenderer->DrawRenderables(renderQueue);

            mRenderer->EndRendering();
        }

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

    ResourceManifest* Engine::bootCreateResourceManifest()
    {
        return mMemoryAllocator.AllocateObject<ResourceManifest>(GetRenderer());
    }


    void Engine::CreateRenderQueue(Scene* scene, std::vector<RenderItem>& renderQueue, const std::vector<LightPtr>& activeLights)
    {
        const std::vector<EntityPtr>& entities = scene->GetAllEntities();
        const Mat4 viewMatrix = scene->GetSceneCamera().GetCameraTransform();
        const Mat4 perspectiveMatrix = CreatePerspectiveMatrix(mWindow->GetScreenMode().FOV, mWindow->GetScreenMode().ScreenWidth / (float)mWindow->GetScreenMode().ScreenHeight, 0.5f, 1000.0f);

        BOOST_FOREACH(EntityPtr entity, entities)
        {
            if (entity->mRender && entity->mModel && entity->mNode)
                CreateModelRenderables(entity->mModel.get(), viewMatrix, perspectiveMatrix, entity->mNode->GetNodeTransform(), renderQueue, activeLights, scene->GetAmbientLight());
        }
    }

    void Engine::GetActiveLights(const Scene* scene, std::vector<LightPtr>& activeLights)
    {
        const std::vector<EntityPtr>& entities = scene->GetAllEntities();
        
        BOOST_FOREACH(EntityPtr entity, entities)
            if (entity && entity->mLight)
                activeLights.push_back(entity->mLight);
    }

    void Engine::CreateModelRenderables(const Model* model, const Mat4& viewMatrix, const Mat4& perspectiveMatrix, const Mat4& nodeTransform, std::vector<RenderItem>& renderQueue, const std::vector<LightPtr>& activeLights, const Vec4& ambientLight)
    {
        const Mat4 worldMatrix = nodeTransform * model->mTransform;
        const Mat4 worldViewMatrix = viewMatrix * worldMatrix;
        const Mat4 worldViewProjMatrix = perspectiveMatrix * worldViewMatrix;

        // TODO: handle multiple lights
        BOOST_FOREACH(const Mesh& mesh, model->mMeshes)
            if (activeLights.size() >= 1)
                renderQueue.push_back(RenderItem(mesh.mVertexBuffer, Vec4(1.0f, 1.0f, 1.0f, 1.0f), worldViewProjMatrix, worldMatrix, activeLights.front()->mLightIntensity, activeLights.front()->mLightDirection, ambientLight));
            else
                renderQueue.push_back(RenderItem(mesh.mVertexBuffer, Vec4(1.0f, 1.0f, 1.0f, 1.0f), worldViewProjMatrix, worldMatrix, Vec4(0.0f), Vec3(0.0f), ambientLight));

        BOOST_FOREACH(const Model& childModel, model->mChildren)
            CreateModelRenderables(&childModel, viewMatrix, perspectiveMatrix, worldMatrix, renderQueue, activeLights, ambientLight);
    }
}