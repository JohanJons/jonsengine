#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/EngineSettings.h"
#include "include/Scene/Scene.h"
#include "include/Window/GLFW/GLFWWindowManager.h"
#include "include/Renderer/OpenGL3/OpenGLRenderer.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/foreach.hpp"
#include <exception>
#include <functional>


namespace JonsEngine
{

    Engine::Engine(const EngineSettings& settings) : mLog(Logger::GetCoreLogger()), mMemoryAllocator("DefaultHeapAllocator", HeapAllocator::DLMALLOC),
                                                     mWindow(settings, std::bind(&Engine::OnContextCreated, this)), 
                                                     mRenderer(mMemoryAllocator.AllocateObject<OpenGLRenderer>(settings, mMemoryAllocator),
                                                        std::bind(&HeapAllocator::DeallocateObject<OpenGLRenderer>, &mMemoryAllocator, std::placeholders::_1)),
                                                     mResourceManifest(mRenderer, mMemoryAllocator), 
                                                     mSceneManager(mResourceManifest)
    {
        JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
    }

    Engine::~Engine()
    {
        JONS_LOG_INFO(mLog, "-------- DESTROYING ENGINE --------")
    }

    void Engine::Tick()
    {
        mWindow.StartFrame();

        mWindow.Poll();

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

        mWindow.EndFrame();
    }

    
    RenderQueue Engine::CreateRenderQueue(const Scene* scene)
    {
        RenderQueue renderQueue;

        const std::vector<ModelPtr>& models = scene->GetResourceManifest().GetAllModels();
        const Mat4 viewMatrix = scene->GetSceneCamera().GetCameraTransform();
        const Mat4 perspectiveMatrix = CreatePerspectiveMatrix(mWindow.GetFOV(), mWindow.GetScreenWidth() / (float)mWindow.GetScreenHeight(), mRenderer->GetZNear(), mRenderer->GetZFar());

        BOOST_FOREACH(ModelPtr model, models)
        {
            if (model && model->mSceneNode)
                CreateModelRenderable(model.get(), viewMatrix, perspectiveMatrix, model->mSceneNode->GetNodeTransform(), model->mLightingEnabled, renderQueue);
        }

        std::sort(renderQueue.begin(), renderQueue.end(), [](const Renderable& smaller, const Renderable& larger) { return smaller.mMesh < larger.mMesh; });

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

        if (model->mMesh != INVALID_MESH_ID)
        {
            // TODO: replace push_back with emplace_back once boost is compatible with MSVC12
            const MaterialPtr material(model->mMaterial);
            if (material)
                renderQueue.push_back(Renderable(model->mMesh, worldViewProjMatrix, worldMatrix, model->mMaterialTilingFactor, lightingEnabled, 0.02f, material->mDiffuseTexture, Vec4(material->mDiffuseColor, 1.0f),
                                         Vec4(material->mAmbientColor, 1.0f), Vec4(material->mSpecularColor, 1.0f), Vec4(material->mEmissiveColor, 1.0f)));
            else
                renderQueue.push_back(Renderable(model->mMesh, worldViewProjMatrix, worldMatrix, model->mMaterialTilingFactor, lightingEnabled, 0.02f));
        }

        BOOST_FOREACH(const Model& childModel, model->mChildren)
            // 'lightingEnabled' is passed on since it applies recursively on all children aswell
            CreateModelRenderable(&childModel, viewMatrix, perspectiveMatrix, worldMatrix, lightingEnabled, renderQueue);
    }


    void Engine::OnContextCreated()
    {
        mRenderer.reset(mMemoryAllocator.AllocateObject<OpenGLRenderer>(*mRenderer));
    }
}