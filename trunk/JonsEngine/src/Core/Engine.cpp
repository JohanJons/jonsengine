#include "include/Core/Engine.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/DebugOptions.h"
#include "include/Scene/Scene.h"
#include "include/Window/GLFW/GLFWWindowManager.h"
#include "include/Renderer/OpenGL3/OpenGLRenderer.h"
#include "include/Resources/ResourceManifest.h"

#include <exception>
#include <functional>


namespace JonsEngine
{
    Engine::Engine(const EngineSettings& settings) : mLog(Logger::GetCoreLogger()), 
                                                     mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator().AllocateObject<HeapAllocator>("DefaultHeapAllocator"), 
                                                                      [](HeapAllocator* allocator) { HeapAllocator::GetDefaultHeapAllocator().DeallocateObject(allocator); }),
                                                     mWindow(settings, std::bind(&Engine::OnContextCreated, this)), 
                                                     mRenderer(mMemoryAllocator->AllocateObject<OpenGLRenderer>(settings, mMemoryAllocator),
                                                               [this](OpenGLRenderer* renderer) { mMemoryAllocator->DeallocateObject(renderer); }),
                                                     mResourceManifest(mRenderer, mMemoryAllocator), 
                                                     mSceneManager(mResourceManifest)
    {
        JONS_LOG_INFO(mLog, "-------- ENGINE INITIALIZED --------")
    }

    Engine::~Engine()
    {
        JONS_LOG_INFO(mLog, "-------- DESTROYING ENGINE --------")
    }


    void Engine::Tick(const DebugOptions& debugOptions)
    {
        mWindow.StartFrame();

        mWindow.Poll();

        if (mSceneManager.HasActiveScene())
        {
            Scene* activeScene = mSceneManager.GetActiveScene();

            // update model matrix of all nodes in active scene
            activeScene->GetRootNode().UpdateModelMatrix(Mat4(1.0f));
            const Camera& camera = activeScene->GetSceneCamera();
            const Mat4 viewMatrix = camera.GetCameraTransform();
            const Mat4 perspectiveMatrix = glm::perspective(mWindow.GetFOV(), mWindow.GetScreenWidth() / (float)mWindow.GetScreenHeight(), mRenderer->GetZNear(), mRenderer->GetZFar());
            const Mat4 viewPerspectiveMatrix = perspectiveMatrix * viewMatrix;

            // create the rendering queue and active lights
            const RenderQueue renderQueue(CreateRenderQueue(activeScene->GetResourceManifest().GetAllModels(), viewPerspectiveMatrix));
            const RenderableLighting lighting(GetLightingInfo(perspectiveMatrix, viewMatrix, viewPerspectiveMatrix, activeScene->GetGamma(), activeScene->GetAmbientLight(), camera.Position(), activeScene->GetPointLights(), activeScene->GetDirectionalLights()));

            // render the scene
            mRenderer->DrawRenderables(renderQueue, lighting, debugOptions.mRenderingMode, debugOptions.mRenderingFlags);
        }

        mWindow.EndFrame();
    }

    
    RenderQueue Engine::CreateRenderQueue(const std::vector<ModelPtr>& models, const Mat4& viewProjectionMatrix)
    {
        RenderQueue renderQueue;

        for(ModelPtr model : models)
        {
            if (model && model->mSceneNode)
                CreateModelRenderable(model.get(), viewProjectionMatrix, model->mSceneNode->GetNodeTransform(), model->mLightingEnabled, renderQueue);
        }

        std::sort(renderQueue.begin(), renderQueue.end(), [](const Renderable& smaller, const Renderable& larger) { return smaller.mMesh < larger.mMesh; });

        return renderQueue;
    }
     
    RenderableLighting Engine::GetLightingInfo(const Mat4& projMatrix, const Mat4& viewMatrix, const Mat4& viewProjectionMatrix, const Vec4& gamma, const Vec4& ambientLight, const Vec3& cameraPosition, const std::vector<PointLightPtr>& pointLights, const std::vector<DirectionalLightPtr>& directionalLights)
    {
        RenderableLighting lighting(viewMatrix, projMatrix, gamma, ambientLight, cameraPosition, Vec2(mWindow.GetScreenWidth(), mWindow.GetScreenHeight()));

        for (PointLightPtr pointLight : pointLights)
        {
            const Mat4 scaledWorldMatrix = glm::scale(pointLight->mSceneNode->GetNodeTransform(), Vec3(pointLight->mMaxDistance));
            lighting.mPointLights.emplace_back(RenderableLighting::PointLight(viewProjectionMatrix * scaledWorldMatrix, scaledWorldMatrix, pointLight->mLightColor, pointLight->mSceneNode->Position(), pointLight->mLightIntensity, pointLight->mMaxDistance));
        }

        for (DirectionalLightPtr dirLight : directionalLights)
            lighting.mDirectionalLights.emplace_back(RenderableLighting::DirectionalLight(dirLight->mLightColor, dirLight->mLightDirection));

        return lighting;
    }

    /*
     * Creates a render unit for model 'model' and all its children.
     */
    void Engine::CreateModelRenderable(const Model* model, const Mat4& viewProjectionMatrix, const Mat4& nodeTransform, const bool lightingEnabled, RenderQueue& renderQueue)
    {
        const Mat4 worldMatrix         = nodeTransform * model->mTransform;
        const Mat4 worldViewProjMatrix = viewProjectionMatrix * worldMatrix;

        if (model->mMesh != INVALID_MESH_ID)
        {
            // TODO: replace push_back with emplace_back once boost is compatible with MSVC12
            const MaterialPtr material(model->mMaterial);
            if (material)
                renderQueue.push_back(Renderable(model->mMesh, worldViewProjMatrix, worldMatrix, model->mMaterialTilingFactor, 
                                                 Vec4(material->mDiffuseColor, 1.0f), Vec4(material->mAmbientColor, 1.0f), Vec4(material->mSpecularColor, 1.0f), Vec4(material->mEmissiveColor, 1.0f),
                                                 material->mDiffuseTexture, material->mNormalTexture, lightingEnabled, material->mSpecularFactor));
            else
                renderQueue.push_back(Renderable(model->mMesh, worldViewProjMatrix, worldMatrix, lightingEnabled));
        }

        for(const Model& childModel : model->mChildren)
            // 'lightingEnabled' is passed on since it applies recursively on all children aswell
            CreateModelRenderable(&childModel, viewProjectionMatrix, worldMatrix, lightingEnabled, renderQueue);
    }


    void Engine::OnContextCreated()
    {
        auto meshes      = mRenderer->GetMeshes();
        auto textures    = mRenderer->GetTextures();
        float anisotropy = mRenderer->GetCurrentAnisotropicFiltering();

        uint32_t shadowMapResolution = mRenderer->GetShadowmapResolution();

        mRenderer.release();
        mRenderer.reset(mMemoryAllocator->AllocateObject<OpenGLRenderer>(meshes, textures, mWindow.GetScreenWidth(), mWindow.GetScreenHeight(), shadowMapResolution, anisotropy, mMemoryAllocator));
    }
} 