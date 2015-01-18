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
                                                     mSceneManager(settings)
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
            mRenderQueue.Clear();

            // update model matrix of all nodes in active scene
            // TODO: only on change in scene node
            activeScene->GetRootNode().UpdateModelMatrix(gIdentityMatrix);

            const Camera& camera = activeScene->GetSceneCamera();
            const Mat4 viewMatrix = camera.GetCameraTransform();
            const Mat4 perspectiveMatrix = PerspectiveMatrixFov(mWindow.GetFOV(), mWindow.GetScreenWidth() / (float)mWindow.GetScreenHeight(), mRenderer.GetZNear(), mRenderer.GetZFar());
            const Mat4 viewPerspectiveMatrix = perspectiveMatrix * viewMatrix;

            // fill the rendering queue
            // TODO: only use active scene --> scene must have models list
            FillRenderQueue(mResourceManifest.GetAllModels(), viewPerspectiveMatrix);

            // get lighting info
            const RenderableLighting lighting(GetLightingInfo(perspectiveMatrix, viewMatrix, viewPerspectiveMatrix, activeScene->GetAmbientLight(), camera.Position(), activeScene->GetPointLights(), activeScene->GetDirectionalLights()));

            // render the scene
            mRenderer.Render(mRenderQueue, lighting, debugOptions.mRenderingFlags);
        }
    }

    void Engine::FillRenderQueue(const std::vector<ModelPtr>& allModels, const Mat4& viewProjectionMatrix)
    {
        // fill camera models
        for (const ModelPtr model : allModels)
        {
            if (!model->mSceneNode)
                continue;

            for (const ModelNode& node : model->mNodes)
            {
                const Mat4& worldMatrix = model->mSceneNode->GetNodeTransform() * model->mInitialTransform;
                CullMeshes(node, viewProjectionMatrix * worldMatrix, worldMatrix);
            }
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

    void Engine::CullMeshes(const ModelNode& node, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.GetTransformMatrix();

        // test node frustrum
        FrustrumIntersection nodeAABBIntersection = IsAABBInFrustum(node.GetAABBCenter(), node.GetAABBExtent(), localWVPMatrix);
        switch (nodeAABBIntersection)
        {
            // if partially inside, recursively test all meshes and child nodes
            case FRUSTRUM_INTERSECTION_PARTIAL:
            {
                FrustrumIntersection meshAABBIntersection(FRUSTRUM_INTERSECTION_INSIDE);

                for (const Mesh& mesh : node.mMeshes)
                {
                    meshAABBIntersection = IsAABBInFrustum(mesh.GetAABBCenter(), mesh.GetAABBExtent(), localWVPMatrix);
                    if (meshAABBIntersection == FRUSTRUM_INTERSECTION_OUTSIDE)
                        continue;

                    if (meshAABBIntersection == FRUSTRUM_INTERSECTION_INSIDE || meshAABBIntersection == FRUSTRUM_INTERSECTION_PARTIAL)
                        AddMesh(mesh, wvpMatrix, worldMatrix);
                }

                for (const ModelNode& node : node.mChildNodes)
                    CullMeshes(node, wvpMatrix, worldMatrix);

                break;
            }

            case FRUSTRUM_INTERSECTION_INSIDE:
            {
                AddAllMeshes(node, wvpMatrix, worldMatrix);
                break;
            }

            case FRUSTRUM_INTERSECTION_OUTSIDE:
            default:
                break;
        }
    }

    void Engine::AddAllMeshes(const ModelNode& node, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.GetTransformMatrix();

        for (const Mesh& mesh : node.mMeshes)
            AddMesh(mesh, localWVPMatrix, worldMatrix);

        for (const ModelNode& node : node.mChildNodes)
            AddAllMeshes(node, wvpMatrix, worldMatrix);
    }

    void Engine::AddMesh(const Mesh& mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        const MaterialPtr material(mesh.mMaterial);
        if (material)
            mRenderQueue.mCameraModels.emplace_back(Renderable(mesh.mMeshID, wvpMatrix, worldMatrix, mesh.mMaterialTilingFactor,
            Vec4(material->mDiffuseColor, 1.0f), Vec4(material->mAmbientColor, 1.0f), Vec4(material->mSpecularColor, 1.0f), Vec4(material->mEmissiveColor, 1.0f),
            material->mDiffuseTexture, material->mNormalTexture, material->mSpecularFactor));
        else
            mRenderQueue.mCameraModels.emplace_back(Renderable(mesh.mMeshID, wvpMatrix, worldMatrix));
    }
}