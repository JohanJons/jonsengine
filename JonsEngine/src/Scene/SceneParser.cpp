#include "include/Scene/SceneParser.h"

#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Core/Types.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Math/Intersection.h"

#include <vector>

namespace JonsEngine
{
    // TODO: some cleanup/refactoring is needed
    // ditch RENDERABLE_TYPE?

    template <typename ACTOR_ITER_TYPE, typename VISIBLITY_FUNC, typename ...VISIBILITY_ARGS>
    void CullActors(const Scene& scene, const ResourceManifest& resManifest, const EngineSettings::CullingStrategy cullingStrat, const ACTOR_ITER_TYPE& actorIterator,
		RenderableMesh::Collection& meshContainer, const VISIBLITY_FUNC& testVisibilityFunc, VISIBILITY_ARGS&&... args);

    template <typename VISIBILITY_RESULT_TYPE>
    bool DetermineIfAddAllMeshes(const EngineSettings::CullingStrategy cullingStrat, const VISIBILITY_RESULT_TYPE visibilityResult);

    AABBIntersection FrustumCull(const Model& model, const Mat4& worldTransform, const Mat4& viewProjectionMatrix);
    AABBIntersection PointLightCull(const Model& model, const Mat4& worldTransform, const Vec3& sphereCentre, const float sphereRadius);
    AABBIntersection DirectionalLightCull(const Model& model, const Mat4& worldTransform, const DirectionalLight::BoundingVolume boundingVolume);

    // there is some juggling with template types due to not having partial template specialization...
    template <typename ACTOR_TYPE>
    void AddAllMeshes(std::vector<RenderableModel>& resultContainer, const AnimationUpdater& AnimationUpdater, const ResourceManifest& resManifest, const ACTOR_TYPE& actor, const Model& model, const Mat4& worldMatrix);
    template <typename ACTOR_TYPE>
    void AddAllMeshes(std::vector<RenderableMesh>& resultContainer, const AnimationUpdater& AnimationUpdater, const ResourceManifest& resManifest, const ACTOR_TYPE& actor, const Model& model, const Mat4& worldMatrix);
    template <typename RENDERABLE_TYPE, typename ...MESH_ARGS>
    void AddMesh(std::vector<RENDERABLE_TYPE>& resultContainer, const AnimationUpdater& AnimationUpdater, const ResourceManifest& resManifest, const StaticActor& actor, const Model& model, const ModelNode& node, const DX11MeshID meshID, const Mat4& worldTransform, MESH_ARGS&&... args);
    template <typename RENDERABLE_TYPE, typename ...MESH_ARGS>
    void AddMesh(std::vector<RENDERABLE_TYPE>& resultContainer, const AnimationUpdater& AnimationUpdater, const ResourceManifest& resManifest, const AnimatedActor& actor, const Model& model, const ModelNode& node, const DX11MeshID meshID, const Mat4& worldTransform, MESH_ARGS&&... args);
    
    void AddStaticMesh();
    void AddAnimatedMesh();


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

		const Mat4 viewMatrix = sceneCamera.GetCameraTransform();
		const Mat4 projMatrix = PerspectiveMatrixFov(cameraFov, windowAspectRatio, zNear, zFar);

        mRenderQueue.mCamera.mFOV = cameraFov;
        mRenderQueue.mCamera.mCameraPosition = sceneCamera.Position();
		mRenderQueue.mCamera.mCameraViewMatrix = viewMatrix;
		mRenderQueue.mCamera.mCameraProjectionMatrix = projMatrix;
        mRenderQueue.mCamera.mCameraViewProjectionMatrix = projMatrix * viewMatrix;

        const auto staticActors = scene.GetStaticActors();
        const auto animatedActors = scene.GetAnimatedActors();

        CullActors<decltype(staticActors)>(scene, mResourceManifest, mCullingStrategy, staticActors, mRenderQueue.mMeshes, FrustumCull, mRenderQueue.mCamera.mCameraViewProjectionMatrix);
        CullActors<decltype(animatedActors)>(scene, mResourceManifest, mCullingStrategy, animatedActors, mRenderQueue.mMeshes, FrustumCull, mRenderQueue.mCamera.mCameraViewProjectionMatrix);
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

        const SkyboxID skyboxID = scene.GetSkybox();
        if (skyboxID != INVALID_SKYBOX_ID)
        {
            const Skybox& skybox = mResourceManifest.GetSkybox(skyboxID);
            mRenderQueue.mSkyboxTextureID = skybox.GetSkyboxTexture();
        }
    }


    template <typename ACTOR_ITER_TYPE, typename VISIBLITY_FUNC, typename ...VISIBILITY_ARGS>
    void CullActors(const Scene& scene, const ResourceManifest& resManifest, const EngineSettings::CullingStrategy cullingStrat, const ACTOR_ITER_TYPE& actorIterator,
		RenderableMesh::Collection& meshContainer, const VISIBLITY_FUNC& testVisibilityFunc, VISIBILITY_ARGS&&... args)
    {
        const AnimationUpdater& AnimationUpdater = scene.GetAnimationUpdater();

        for (const auto& actor : actorIterator)
        {
            const SceneNodeID sceneNodeID = actor.GetSceneNode();
            const ModelID modelID = actor.GetModel();
            if (sceneNodeID == INVALID_SCENE_NODE_ID || modelID == INVALID_MODEL_ID)
                continue;

            const SceneNode& sceneNode = scene.GetSceneNode(sceneNodeID);
            const Model& model = resManifest.GetModel(modelID);
            const Mat4& worldMatrix = sceneNode.GetWorldTransform();

            const auto visibilityResult = testVisibilityFunc(model, worldMatrix, std::forward<VISIBILITY_ARGS>(args)...);

            // TODO: extend with fine-grained culling, such as adding meshes per-node rather than per-model for AGGRESSIVE culling strategy,
            const bool addAllMeshes = DetermineIfAddAllMeshes(cullingStrat, visibilityResult);
            if (addAllMeshes)
                AddAllMeshes<decltype(actor)>(meshContainer, AnimationUpdater, resManifest, actor, model, worldMatrix);
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


    AABBIntersection FrustumCull(const Model& model, const Mat4& worldTransform, const Mat4& viewProjMatrix)
    {
        const Mat4 wvpMatrix = viewProjMatrix * worldTransform;
        const AABB modelAABB = model.GetStaticAABB();

        return Intersection(modelAABB, wvpMatrix);
    }

    AABBIntersection PointLightCull(const Model& model, const Mat4& worldTransform, const Vec3& sphereCentre, const float sphereRadius)
    {
        const AABB worldAABB = worldTransform * model.GetStaticAABB();

        return Intersection(worldAABB, sphereCentre, sphereRadius);
    }

    AABBIntersection DirectionalLightCull(const Model& model, const Mat4& worldTransform, const DirectionalLight::BoundingVolume boundingVolume)
    {
        const AABB worldAABB = worldTransform * model.GetStaticAABB();

        return Intersection(worldAABB, boundingVolume);
    }


    template <typename ACTOR_TYPE>
    void AddAllMeshes(std::vector<RenderableModel>& resultContainer, const AnimationUpdater& AnimationUpdater, const ResourceManifest& resManifest, const ACTOR_TYPE& actor, const Model& model, const Mat4& worldTransform)
    {
        for (const ModelNode& node : model.GetNodes())
        {
            if (node.GetNumMeshes() == 0)
                continue;

            for (const Mesh& mesh : node.GetMeshes())
            {
                auto diffuseTexture = INVALID_DX11_MATERIAL_ID;
                auto normalTexture = INVALID_DX11_MATERIAL_ID;
                // TODO
                auto specularFactor = 0.02f;

                MaterialID materialID = mesh.GetDefaultMaterial();
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

                AddMesh<RenderableModel>(resultContainer, AnimationUpdater, resManifest, actor, model, node, mesh.GetMesh(), worldTransform, diffuseTexture, normalTexture, specularFactor, actor.GetMaterialTilingFactor());
            }
        }
    }

    template <typename ACTOR_TYPE>
    void AddAllMeshes(std::vector<RenderableMesh>& resultContainer, const AnimationUpdater& AnimationUpdater, ResourceManifest& resManifest, const ACTOR_TYPE& actor, const Model& model, const Mat4& worldTransform)
    {
        for (const ModelNode& node : model.GetNodes())
        {
            if (node.GetNumMeshes() == 0)
                continue;

            for (const Mesh& mesh : node.GetMeshes())
                AddMesh<RenderableMesh>(resultContainer, AnimationUpdater, resManifest, actor, model, node, mesh.GetMesh(), worldTransform);
        }
    }

    template <typename RENDERABLE_TYPE, typename ...MESH_ARGS>
    void AddMesh(std::vector<RENDERABLE_TYPE>& resultContainer, const AnimationUpdater& AnimationUpdater, const ResourceManifest& resManifest, const StaticActor& actor, const Model& model, const ModelNode& node, const DX11MeshID meshID, const Mat4& worldTransform, MESH_ARGS&&... args)
    {
        const bool isAnimating = false;

        resultContainer.emplace_back(meshID, worldTransform, isAnimating, std::forward<MESH_ARGS>(args)...);
    }

    template <typename RENDERABLE_TYPE, typename ...MESH_ARGS>
    void AddMesh(std::vector<RENDERABLE_TYPE>& resultContainer, const AnimationUpdater& AnimationUpdater, const ResourceManifest& resManifest, const AnimatedActor& actor, const Model& model, const ModelNode& node, const DX11MeshID meshID, const Mat4& worldTransform, MESH_ARGS&&... args)
    {
        const bool isAnimating = actor.IsPlaying();

        if (isAnimating)
        {
            const auto animInstanceID = actor.GetAnimationInstance();
            const auto& boneData = AnimationUpdater.GetBoneData(animInstanceID);
            
            resultContainer.emplace_back(meshID, worldTransform, isAnimating, std::forward<MESH_ARGS>(args)...);
        }
        else
            resultContainer.emplace_back(meshID, worldTransform, isAnimating, std::forward<MESH_ARGS>(args)...);
    }
}