#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/EngineSettings.h"
#include "include/Scene/Scene.h"
#include "include/Window/GLFW/GLFWWindow.h"
#include "include/Window/DummyWindow.h"
#include "include/Renderer/OpenGL3/OpenGLRenderer.h"
#include "include/Renderer/DummyRenderer.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/foreach.hpp"


namespace JonsEngine
{

    Engine::Engine(const EngineSettings& settings) : mLog(Logger::GetCoreLogger()), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), 
                                                     mWindow(bootCreateWindow(settings)), mRenderer(bootCreateRenderer(settings)), mInputManager(settings),
                                                     mResourceManifest(bootCreateResourceManifest()), mSceneManager(*mResourceManifest)
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
            const RenderQueue renderQueue(CreateRenderQueue(activeScene));
            const RenderableLighting lighting(GetLightingInfo(activeScene));

            // render the scene
            mRenderer->DrawRenderables(renderQueue, lighting);
        }

        mWindow->EndFrame();
    }


    IWindow* Engine::bootCreateWindow(const EngineSettings& settings)
    {
        switch (settings.WindowBackend)
        {
            case IWindow::GLFW:
                return mMemoryAllocator.AllocateObject<GLFWWindow>(settings);

            case IWindow::NONE:
            default:
                return mMemoryAllocator.AllocateObject<DummyWindow>(settings);
        }
    }

    IRenderer* Engine::bootCreateRenderer(const EngineSettings& engineSettings)
    {
        switch (engineSettings.RenderBackend)
        {
            case IRenderer::OPENGL:
                return mMemoryAllocator.AllocateObject<OpenGLRenderer>(engineSettings);

            case IRenderer::NONE:
            default:
                return mMemoryAllocator.AllocateObject<DummyRenderer>(engineSettings);
        }
    }

    ResourceManifest* Engine::bootCreateResourceManifest()
    {
        return mMemoryAllocator.AllocateObject<ResourceManifest, IRenderer&>(GetRenderer());
    }

    
    RenderQueue Engine::CreateRenderQueue(const Scene* scene)
    {
        RenderQueue renderQueue;
        const ScreenMode& screenMode = mWindow->GetScreenMode();

        const std::vector<ModelPtr>& models = scene->GetResourceManifest().GetAllModels();
        const Mat4 viewMatrix = scene->GetSceneCamera().GetCameraTransform();
        const Mat4 perspectiveMatrix = CreatePerspectiveMatrix(screenMode.FOV, screenMode.ScreenWidth / (float)screenMode.ScreenHeight, screenMode.zNear, screenMode.zFar);

        BOOST_FOREACH(ModelPtr model, models)
        {
            if (model && model->mSceneNode)
                CreateModelRenderable(model.get(), viewMatrix, perspectiveMatrix, model->mSceneNode->GetNodeTransform(), model->mLightingEnabled, renderQueue);
        }

        return renderQueue;
    }
     
    // TODO: Cull lights
    RenderableLighting Engine::GetLightingInfo(const Scene* scene)
    {
        const std::vector<LightPtr>& sceneLights = scene->GetAllLights();

        RenderableLighting lighting(scene->GetGamma(), scene->GetSceneCamera().Forward(), sceneLights.size());

        int lightIndex = 0;
        BOOST_FOREACH(LightPtr light, sceneLights)
        {
            lighting.mLights[lightIndex].mLightColor     = light->mLightColor;
            lighting.mLights[lightIndex].mLightPosition  = Vec4(light->mSceneNode->Position(), 1.0f);
            lighting.mLights[lightIndex].mLightDirection = Vec4(light->mLightDirection, 0.0f);
            lighting.mLights[lightIndex].mLightType      = light->mLightType;
            lighting.mLights[lightIndex].mIntensity      = light->mIntensity;
            lighting.mLights[lightIndex].mRadius         = light->mRadius;
            lighting.mLights[lightIndex].mMaxDistance    = light->mMaxDistance;

            lightIndex++;
        }

        return lighting;
    }

    /*
     * Creates a render unit for model 'model' and all its children.
     */
    void Engine::CreateModelRenderable(const Model* model, const Mat4& viewMatrix, const Mat4& perspectiveMatrix, const Mat4& nodeTransform, const bool lightingEnabled, RenderQueue& renderQueue)
    {
        const Mat4 worldMatrix         = nodeTransform * model->mTransform;
        const Mat4 worldViewMatrix     = viewMatrix * worldMatrix;
        const Mat4 worldViewProjMatrix = perspectiveMatrix * worldViewMatrix;

        if (model->mMesh)
        {
            Renderable renderable(model->mMesh, worldViewProjMatrix, worldMatrix, model->mMaterialTilingFactor, lightingEnabled, 0.02f);
            const MaterialPtr material(model->mMaterial);

            if (material)
            {
                renderable.mDiffuseTexture = material->mDiffuseTexture;
                renderable.mDiffuseColor   = Vec4(material->mDiffuseColor, 1.0f);
                renderable.mAmbientColor   = Vec4(material->mAmbientColor, 1.0f);
                renderable.mSpecularColor  = Vec4(material->mSpecularColor, 1.0f);
                renderable.mEmissiveColor  = Vec4(material->mEmissiveColor, 1.0f);
            }

            renderQueue.push_back(renderable);     // TODO: Add specularfactor to model/mesh
        }

        BOOST_FOREACH(const Model& childModel, model->mChildren)
            // 'lightingEnabled' is passed on since it applies recursively on all children aswell
            CreateModelRenderable(&childModel, viewMatrix, perspectiveMatrix, worldMatrix, lightingEnabled, renderQueue);
    }
}