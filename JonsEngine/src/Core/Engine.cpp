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
        mWindow.Poll();

        Scene* activeScene = mSceneManager.GetActiveScene();

        if (activeScene)
        {
            // reset render queue
            mRenderQueue.clear();

            // update model matrix of all nodes in active scene
            activeScene->GetRootNode().UpdateModelMatrix(Mat4(1.0f));

            const Camera& camera = activeScene->GetSceneCamera();
            const Mat4 viewMatrix = camera.GetCameraTransform();
            const Mat4 perspectiveMatrix = PerspectiveMatrixFov(mWindow.GetFOV(), mWindow.GetScreenWidth() / (float)mWindow.GetScreenHeight(), mRenderer.GetZNear(), mRenderer.GetZFar());
            const Mat4 viewPerspectiveMatrix = perspectiveMatrix * viewMatrix;

            // fill the rendering queue
            FillRenderQueue(activeScene->GetResourceManifest().GetAllModels(), viewPerspectiveMatrix);

            // get lighting info
            const RenderableLighting lighting(GetLightingInfo(perspectiveMatrix, viewMatrix, viewPerspectiveMatrix, activeScene->GetAmbientLight(), camera.Position(), activeScene->GetPointLights(), activeScene->GetDirectionalLights()));

            // render the scene
            mRenderer.Render(mRenderQueue, lighting, debugOptions.mRenderingFlags);
        }
    }

    
    void Engine::FillRenderQueue(const std::vector<ModelPtr>& models, const Mat4& viewProjectionMatrix)
    {
        for(ModelPtr model : models)
        {
            if (model && model->mSceneNode)
                CreateModelRenderable(model.get(), viewProjectionMatrix, model->mSceneNode->GetNodeTransform(), model->mLightingEnabled);
        }

        std::sort(mRenderQueue.begin(), mRenderQueue.end(), [](const Renderable& smaller, const Renderable& larger) { return smaller.mMesh < larger.mMesh; });
    }
     
    RenderableLighting Engine::GetLightingInfo(const Mat4& projMatrix, const Mat4& viewMatrix, const Mat4& viewProjectionMatrix, const Vec4& ambientLight, const Vec3& cameraPosition, const std::vector<PointLightPtr>& pointLights, const std::vector<DirectionalLightPtr>& directionalLights)
    {
        RenderableLighting lighting(viewMatrix, projMatrix, ambientLight, cameraPosition, mWindow.GetFOV());

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
    void Engine::CreateModelRenderable(const Model* model, const Mat4& viewProjectionMatrix, const Mat4& nodeTransform, const bool lightingEnabled)
    {
        const Mat4 worldMatrix         = nodeTransform * model->mTransform;
        const Mat4 worldViewProjMatrix = viewProjectionMatrix * worldMatrix;

        if (model->mMesh != INVALID_MESH_ID)
        {
            const MaterialPtr material(model->mMaterial);
            if (material)
                mRenderQueue.emplace_back(Renderable(model->mMesh, worldViewProjMatrix, worldMatrix, model->mMaterialTilingFactor,
                                                    Vec4(material->mDiffuseColor, 1.0f), Vec4(material->mAmbientColor, 1.0f), Vec4(material->mSpecularColor, 1.0f), Vec4(material->mEmissiveColor, 1.0f),
                                                     material->mDiffuseTexture, material->mNormalTexture, lightingEnabled, material->mSpecularFactor));
            else
                mRenderQueue.emplace_back(Renderable(model->mMesh, worldViewProjMatrix, worldMatrix, lightingEnabled));
        }

        for(const Model& childModel : model->mChildren)
            // 'lightingEnabled' is passed on since it applies recursively on all children aswell
            CreateModelRenderable(&childModel, viewProjectionMatrix, worldMatrix, lightingEnabled);
    }
} 