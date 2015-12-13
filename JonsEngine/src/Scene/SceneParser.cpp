#include "include/Scene/SceneParser.h"

#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Core/Types.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Math/Intersection.h"

#include <vector>

namespace JonsEngine
{
    template <typename ACTOR_ITER_TYPE, typename RENDERABLE_TYPE, typename VISIBLITY_FUNC, typename ...VISIBILITY_ARGS>
    void CullActors(const Scene& scene, const ResourceManifest& resManifest, const EngineSettings::CullingStrategy cullingStrat, const ACTOR_ITER_TYPE& actorIterator,
        std::vector<RENDERABLE_TYPE>& meshContainer, const VISIBLITY_FUNC& testVisibilityFunc, VISIBILITY_ARGS&&... args);

    template <typename VISIBILITY_RESULT_TYPE>
    bool DetermineIfAddAllMeshes(const EngineSettings::CullingStrategy cullingStrat, const VISIBILITY_RESULT_TYPE visibilityResult);

    AABBIntersection FrustumCull(const ModelNode& node, const Mat4& worldTransform, const Mat4& viewProjectionMatrix);
    AABBIntersection PointLightCull(const ModelNode& node, const Mat4& worldTransform, const Vec3& sphereCentre, const float sphereRadius);
    AABBIntersection DirectionalLightCull(const ModelNode& node, const Mat4& worldTransform, const DirectionalLight::BoundingVolume boundingVolume);

    template <typename RENDERABLE, typename ACTOR_TYPE>
    void AddAllMeshes(const ResourceManifest& resManifest, std::vector<RENDERABLE>& resultContainer, const ACTOR_TYPE& actor, const Model& model, const Mat4& worldMatrix);


    SceneParser::SceneParser(const EngineSettings& engineSettings, const ResourceManifest& resManifest) : mResourceManifest(resManifest), mCullingStrategy(engineSettings.mSceneCullingStrategy)
    {
    }

    SceneParser::~SceneParser()
    {
    }


    const RenderQueue& SceneParser::ParseScene(const Scene& scene, const float windowAspectRatio, const float zNear, const float zFar)
    {
        mRenderQueue.Clear();

        ViewFrustumCulling(scene, windowAspectRatio, zNear, zFar);
        PointLightCulling(scene);
        DirectionalLightCulling(scene);
        ParseMiscSceneInfo(scene);

        return mRenderQueue;
    }


    void SceneParser::ViewFrustumCulling(const Scene& scene, const float windowAspectRatio, const float zNear, const float zFar)
    {
        const Camera& sceneCamera = scene.GetSceneCamera();
        const float cameraFov = sceneCamera.GetFOV();

        mRenderQueue.mCamera.mFOV = cameraFov;
        mRenderQueue.mCamera.mCameraPosition = sceneCamera.Position();
        mRenderQueue.mCamera.mCameraViewMatrix = sceneCamera.GetCameraTransform();
        mRenderQueue.mCamera.mCameraProjectionMatrix = PerspectiveMatrixFov(cameraFov, windowAspectRatio, zNear, zFar);
        mRenderQueue.mCamera.mCameraViewProjectionMatrix = mRenderQueue.mCamera.mCameraProjectionMatrix * mRenderQueue.mCamera.mCameraViewMatrix;

        const auto staticActors = scene.GetStaticActors();
        const auto animatedActors = scene.GetAnimatedActors();

        CullActors<decltype(staticActors), RenderableModel>(scene, mResourceManifest, mCullingStrategy, staticActors, mRenderQueue.mCamera.mModels, FrustumCull, mRenderQueue.mCamera.mCameraViewProjectionMatrix);
        CullActors<decltype(animatedActors), RenderableModel>(scene, mResourceManifest, mCullingStrategy, animatedActors, mRenderQueue.mCamera.mModels, FrustumCull, mRenderQueue.mCamera.mCameraViewProjectionMatrix);
    }

    void SceneParser::PointLightCulling(const Scene& scene)
    {
        const auto staticActors = scene.GetStaticActors();
        const auto animatedActors = scene.GetAnimatedActors();

        for (const PointLight& pointLight : scene.GetPointLights())
        {
            if (pointLight.GetSceneNode() == INVALID_SCENE_NODE_ID)
                continue;

            const SceneNode& sceneNode = scene.GetSceneNode(pointLight.GetSceneNode());
            const Vec3 lightPosition = sceneNode.Position();
            const float lightRadius = pointLight.GetRadius();
            const float lightIntensity = pointLight.GetIntensity();
            const Vec4& lightColor = pointLight.GetLightColor();

            mRenderQueue.mPointLights.emplace_back(lightColor, lightPosition, lightIntensity, lightRadius);
            RenderablePointLight& renderablePointLight = mRenderQueue.mPointLights.back();

            CullActors<decltype(staticActors), RenderableMesh>(scene, mResourceManifest, mCullingStrategy, staticActors, renderablePointLight.mMeshes, PointLightCull, lightPosition, lightRadius);
            CullActors<decltype(animatedActors), RenderableMesh>(scene, mResourceManifest, mCullingStrategy, animatedActors, renderablePointLight.mMeshes, PointLightCull, lightPosition, lightRadius);
        }
    }

    void SceneParser::DirectionalLightCulling(const Scene& scene)
    {
        const auto staticActors = scene.GetStaticActors();
        const auto animatedActors = scene.GetAnimatedActors();

        for (const DirectionalLight& dirLight : scene.GetDirectionalLights())
        {
            mRenderQueue.mDirectionalLights.emplace_back(dirLight.GetLightColor(), glm::normalize(dirLight.GetLightDirection()), dirLight.GetNumCascades());
            RenderableDirLight& renderableDirLight = mRenderQueue.mDirectionalLights.back();

            for (uint32_t cascadeIndex = 0; cascadeIndex < dirLight.GetNumCascades(); ++cascadeIndex)
            {
                float nearZ = 0.0f, farZ = 0.0f;
                dirLight.GetSplitDistance(cascadeIndex, nearZ, farZ);
                const auto kdopIterator = dirLight.GetBoundingVolume(cascadeIndex);

                CullActors<decltype(staticActors), RenderableMesh>(scene, mResourceManifest, mCullingStrategy, staticActors, renderableDirLight.mMeshes, DirectionalLightCull, kdopIterator);
                CullActors<decltype(animatedActors), RenderableMesh>(scene, mResourceManifest, mCullingStrategy, animatedActors, renderableDirLight.mMeshes, DirectionalLightCull, kdopIterator);

                renderableDirLight.mCascadeSplits.emplace_back(nearZ, farZ, renderableDirLight.mMeshes.size());
            }
        }
    }

    void SceneParser::ParseMiscSceneInfo(const Scene& scene)
    {
        mRenderQueue.mAmbientLight = scene.GetAmbientLight();

        const Skybox& skybox = mResourceManifest.GetSkybox(scene.GetSkybox());
        mRenderQueue.mSkyboxTextureID = skybox.GetSkyboxTexture();
    }


    template <typename ACTOR_ITER_TYPE, typename RENDERABLE_TYPE, typename VISIBLITY_FUNC, typename ...VISIBILITY_ARGS>
    void CullActors(const Scene& scene, const ResourceManifest& resManifest, const EngineSettings::CullingStrategy cullingStrat, const ACTOR_ITER_TYPE& actorIterator,
        std::vector<RENDERABLE_TYPE>& meshContainer, const VISIBLITY_FUNC& testVisibilityFunc, VISIBILITY_ARGS&&... args)
    {
        for (const auto& actor : actorIterator)
        {
            const SceneNodeID sceneNodeID = actor.GetSceneNode();
            const ModelID modelID = actor.GetModel();
            if (sceneNodeID == INVALID_SCENE_NODE_ID || modelID == INVALID_MODEL_ID)
                continue;

            const SceneNode& sceneNode = scene.GetSceneNode(sceneNodeID);
            const Model& model = resManifest.GetModel(modelID);
            const Mat4& worldMatrix = sceneNode.GetWorldTransform();

            const auto visibilityResult = testVisibilityFunc(model.GetRootNode(), worldMatrix, std::forward<VISIBILITY_ARGS>(args)...);

            // TODO: extend with fine-grained culling, such as adding meshes per-node rather than per-model for AGGRESSIVE culling strategy,
            const bool addAllMeshes = DetermineIfAddAllMeshes(cullingStrat, visibilityResult);
            if (addAllMeshes)
                AddAllMeshes<RENDERABLE_TYPE>(resManifest, meshContainer, actor, model, worldMatrix);
            //    AddAllMeshes<RENDERABLE_TYPE>(model, renderQueue.mCamera.mModels, resManifest, worldMatrix, actor.GetMaterial(), actor.GetMaterialTilingFactor());
        }
    }


    template <>
    bool DetermineIfAddAllMeshes<AABBIntersection>(const EngineSettings::CullingStrategy cullingStrat, const AABBIntersection aabbIntersection)
    {
        switch (cullingStrat)
        {
            case EngineSettings::CullingStrategy::STANDARD:
            case EngineSettings::CullingStrategy::AGGRESSIVE:
                return aabbIntersection == AABBIntersection::Inside || aabbIntersection == AABBIntersection::Partial;

            default:
                return false;
        }
    }


    AABBIntersection FrustumCull(const ModelNode& node, const Mat4& worldTransform, const Mat4& viewProjMatrix)
    {
        const Mat4& localTransform = node.GetLocalTransform();
        const Mat4 localWVPMatrix = viewProjMatrix * worldTransform * localTransform;

        return Intersection(node.GetLocalAABB(), localWVPMatrix);
    }

    AABBIntersection PointLightCull(const ModelNode& node, const Mat4& worldTransform, const Vec3& sphereCentre, const float sphereRadius)
    {
        const Mat4 localWorldMatrix = worldTransform * node.GetLocalTransform();
        const AABB nodeWorldAABB = localWorldMatrix * node.GetLocalAABB();

        return Intersection(nodeWorldAABB, sphereCentre, sphereRadius);
    }

    AABBIntersection DirectionalLightCull(const ModelNode& node, const Mat4& worldTransform, const DirectionalLight::BoundingVolume boundingVolume)
    {
        const Mat4 localWorldMatrix = worldTransform * node.GetLocalTransform();
        const AABB nodeWorldAABB = localWorldMatrix * node.GetLocalAABB();

        return Intersection(nodeWorldAABB, boundingVolume);
    }


    template <typename ACTOR_TYPE>
    void AddAllMeshes<RenderableModel, ACTOR_TYPE>(const ResourceManifest& resManifest, std::vector<RenderableModel>& resultContainer, const ACTOR_TYPE& actor, const Model& model, const Mat4& worldMatrix)
    {
        for (const Mesh& mesh : model.GetMeshes())
        {
            auto diffuseTexture = INVALID_DX11_MATERIAL_ID;
            auto normalTexture = INVALID_DX11_MATERIAL_ID;
            // TODO
            auto specularFactor = 0.02f;

            MaterialID materialID = actor.GetMaterial();
            const MaterialID explicitActorMaterial = actor.GetMaterial();
            if (explicitActorMaterial != INVALID_MATERIAL_ID)
                materialID = explicitActorMaterial;

            if (materialID != INVALID_MATERIAL_ID)
            {
                const Material& material = resManifest.GetMaterial(materialID);
                diffuseTexture = material.GetDiffuseTexture();
                normalTexture = material.GetNormalTexture();
                specularFactor = material.GetSpecularFactor();
            }

            resultModels.emplace_back(mesh.GetMesh(), worldMatrix, diffuseTexture, normalTexture, specularFactor, tilingFactor);
        }
    }

    /*template <>
    void AddAllMeshes<RenderableModel>(const Model& model, std::vector<RenderableModel>& resultModels, const ResourceManifest& resManifest, const Mat4& worldMatrix, const MaterialID explicitActorMaterial, const float tilingFactor)
    {
        for (const Mesh& mesh : model.GetMeshes())
        {
            auto diffuseTexture = INVALID_DX11_MATERIAL_ID;
            auto normalTexture = INVALID_DX11_MATERIAL_ID;
            // TODO
            auto specularFactor = 0.02f;

            const bool actorHasMaterial = explicitActorMaterial != INVALID_MATERIAL_ID;
            const MaterialID materialID = actorHasMaterial ? explicitActorMaterial : mesh.GetDefaultMaterial();
            if (materialID != INVALID_MATERIAL_ID)
            {
                const Material& material = resManifest.GetMaterial(materialID);
                diffuseTexture = material.GetDiffuseTexture();
                normalTexture = material.GetNormalTexture();
                specularFactor = material.GetSpecularFactor();
            }

            resultModels.emplace_back(mesh.GetMesh(), worldMatrix, diffuseTexture, normalTexture, specularFactor, tilingFactor);
        }
    }

    template <>
    void AddAllMeshes<RenderableMesh>(const Model& model, std::vector<RenderableMesh>& resultMeshes, const ResourceManifest& resManifest, const Mat4& worldMatrix, const MaterialID explicitActorMaterial, const float tilingFactor)
    {
        for (const Mesh& mesh : model.GetMeshes())
            resultMeshes.emplace_back(mesh.GetMesh(), worldMatrix);
    }*/

    /*template <typename RENDERABLE_TYPE, typename ACTOR_ITER_TYPE>
    void FrustumCull(const Scene& scene, RenderQueue& renderQueue, const ResourceManifest& resManifest, const ACTOR_ITER_TYPE& actorIterator)
    {
        for (const auto& actor : actorIterator)
        {
            const SceneNodeID sceneNodeID = actor.GetSceneNode();
            const ModelID modelID = actor.GetModel();
            if (sceneNodeID == INVALID_SCENE_NODE_ID || modelID == INVALID_MODEL_ID)
                continue;

            const SceneNode& sceneNode = scene.GetSceneNode(sceneNodeID);
            const Model& model = resManifest.GetModel(modelID);

            const Mat4& worldMatrix = sceneNode.GetWorldTransform();
            const Mat4 wvpMatrix = renderQueue.mCamera.mCameraViewProjectionMatrix * worldMatrix;

            if (IsNodeVisible(wvpMatrix, worldMatrix, model.GetRootNode()))
                AddAllMeshes<RENDERABLE_TYPE>(model, renderQueue.mCamera.mModels, resManifest, worldMatrix, actor.GetMaterial(), actor.GetMaterialTilingFactor());
        }
    }*/
}