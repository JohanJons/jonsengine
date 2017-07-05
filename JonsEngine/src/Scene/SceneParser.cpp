#include "include/Scene/SceneParser.h"

#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Core/Types.h"
#include "include/Core/DebugOptions.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Math/Intersection.h"

#include <vector>

namespace JonsEngine
{
    // TODO: some cleanup/refactoring is needed

    template <typename ACTOR_ITER_TYPE, typename VISIBLITY_FUNC, typename ...VISIBILITY_ARGS>
	void CullActors(const Scene& scene, const ResourceManifest& resManifest, const EngineSettings::CullingStrategy cullingStrat, const ACTOR_ITER_TYPE& actorIterator, const bool parseMaterials,
		RenderQueue::RenderData& renderData, const VISIBLITY_FUNC& testVisibilityFunc, VISIBILITY_ARGS&&... args);

	//template <typename VISIBLITY_FUNC, typename ...VISIBILITY_ARGS>
	//void CullTerrain(const Scene& scene, const ResourceManifest& resManifest, const EngineSettings::CullingStrategy cullingStrat, const ACTOR_ITER_TYPE& actorIterator,
	//	RenderQueue::RenderData& renderData, const VISIBLITY_FUNC& testVisibilityFunc, VISIBILITY_ARGS&&... args);

    template <typename VISIBILITY_RESULT_TYPE>
    bool DetermineIfAddAllMeshes(const EngineSettings::CullingStrategy cullingStrat, const VISIBILITY_RESULT_TYPE visibilityResult);

    AABBIntersection FrustumCull(const Model& model, const Mat4& worldTransform, const Mat4& viewProjectionMatrix);
    AABBIntersection PointLightCull(const Model& model, const Mat4& worldTransform, const Vec3& sphereCentre, const float sphereRadius);
    AABBIntersection DirectionalLightCull(const Model& model, const Mat4& worldTransform, const DirectionalLight::BoundingVolume boundingVolume);

	template <typename ACTOR_TYPE>
	void AddAllMeshes(const ACTOR_TYPE& actor, const Model& model, const Mat4& worldMatrix, const bool parseMaterials, const AnimationUpdater& AnimationUpdater, const ResourceManifest& resManifest,
		RenderQueue::RenderData& renderData);

	template <typename ACTOR_TYPE>
    void AddMesh(const ACTOR_TYPE& actor, const Mat4& worldTransform, const Mat4& localTransform, const RenderableMaterial::Index materialIndex, const float materialTilingFactor, const DX11MeshID mesh, const AnimationUpdater& animationUpdater, RenderQueue::RenderData& renderData);

	template <typename ACTOR_TYPE>
	RenderableMaterial::Index ParseMaterial(const ACTOR_TYPE& actor, const Mesh& mesh, const bool parseMaterial, const ResourceManifest& resManifest, RenderableMaterial::ContainerType& materialContainer);

	template <typename ACTOR_ITER_TYPE>
	void CullAABB(const Scene& scene, const ResourceManifest& resManifest, const ACTOR_ITER_TYPE& actorIterator, const EngineSettings::CullingStrategy cullingStrat, AABBRenderData::RenderableAABBsContainer& aabbDataContainer,
		const Mat4& viewProjTransform);


    SceneParser::SceneParser(const EngineSettings& engineSettings, const ResourceManifest& resManifest) :
		mResourceManifest(resManifest),
		mCullingStrategy(engineSettings.mSceneCullingStrategy)
    {
    }

    SceneParser::~SceneParser()
    {
    }


    const RenderQueue& SceneParser::ParseScene(const Scene& scene, const DebugOptions& debugOpts, const float windowAspectRatio, const float zNear, const float zFar)
    {
        mRenderQueue.Clear();

		GetAmbientLight(scene);
		GetSkybox(scene);
		CopyBoneTransforms(scene);

        ViewFrustumCulling(scene, windowAspectRatio, zNear, zFar);
        PointLightCulling(scene);
        DirectionalLightCulling(scene);

		if (debugOpts.mRenderingFlags.test(debugOpts.RENDER_FLAG_DRAW_AABB))
			AddAABBDebugData(scene);

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

		mRenderQueue.mCamera.mStaticMeshesBegin = mRenderQueue.mRenderData.mStaticMeshes.size();
		mRenderQueue.mCamera.mAnimatedMeshesBegin = mRenderQueue.mRenderData.mAnimatedMeshes.size();

        const auto staticActors = scene.GetStaticActors();
        const auto animatedActors = scene.GetAnimatedActors();
		bool parseMaterials = true;

        CullActors<decltype(staticActors)>(scene, mResourceManifest, mCullingStrategy, staticActors, parseMaterials, mRenderQueue.mRenderData, FrustumCull, mRenderQueue.mCamera.mCameraViewProjectionMatrix);
        CullActors<decltype(animatedActors)>(scene, mResourceManifest, mCullingStrategy, animatedActors, parseMaterials, mRenderQueue.mRenderData, FrustumCull, mRenderQueue.mCamera.mCameraViewProjectionMatrix);

		mRenderQueue.mCamera.mStaticMeshesEnd =  mRenderQueue.mRenderData.mStaticMeshes.size();
		mRenderQueue.mCamera.mAnimatedMeshesEnd = mRenderQueue.mRenderData.mAnimatedMeshes.size();

		const auto terrains = scene.GetTerrains();
		//CullTerrain<decltype(terrains)>(scene, mResourceManifest, mCullingStrategy, terrains, mRenderQueue.mRenderData, FrustumCull, mRenderQueue.mCamera.mCameraViewProjectionMatrix);
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
			const Vec4& color = pointLight.GetLightColor();
            const Vec3 position = sceneNode.Position();
            const float intensity = pointLight.GetIntensity();
			const float radius = pointLight.GetRadius();

            mRenderQueue.mPointLights.emplace_back(color, position, intensity, radius);
            RenderablePointLight& renderablePointLight = mRenderQueue.mPointLights.back();

			renderablePointLight.mStaticMeshesBegin = mRenderQueue.mRenderData.mStaticMeshes.size();
			renderablePointLight.mAnimatedMeshesBegin = mRenderQueue.mRenderData.mAnimatedMeshes.size();

			// dont need materials for shadowmapping
			const bool parseMaterials = false;

            CullActors<decltype(staticActors)>(scene, mResourceManifest, mCullingStrategy, staticActors, parseMaterials, mRenderQueue.mRenderData, PointLightCull, position, radius);
            CullActors<decltype(animatedActors)>(scene, mResourceManifest, mCullingStrategy, animatedActors, parseMaterials, mRenderQueue.mRenderData, PointLightCull, position, radius);

			renderablePointLight.mStaticMeshesEnd = mRenderQueue.mRenderData.mStaticMeshes.size();
			renderablePointLight.mAnimatedMeshesEnd = mRenderQueue.mRenderData.mAnimatedMeshes.size();
        }
    }

    void SceneParser::DirectionalLightCulling(const Scene& scene)
    {
        const auto staticActors = scene.GetStaticActors();
        const auto animatedActors = scene.GetAnimatedActors();

        for (const DirectionalLight& dirLight : scene.GetDirectionalLights())
        {
            mRenderQueue.mDirectionalLights.emplace_back(dirLight.GetLightColor(), glm::normalize(dirLight.GetLightDirection()), dirLight.GetNumCascades());
            RenderableDirectionalLight& renderableDirLight = mRenderQueue.mDirectionalLights.back();

			// dont need materials for shadowmapping
			const bool parseMaterials = false;

			const uint32_t numCascades = dirLight.GetNumCascades();
            for (uint32_t cascadeIndex = 0; cascadeIndex < numCascades; ++cascadeIndex)
            {
                float nearZ = 0.0f, farZ = 0.0f;
                dirLight.GetSplitDistance(cascadeIndex, nearZ, farZ);
				auto& cascade = renderableDirLight.mCascades.at(cascadeIndex);
				cascade.mNearZ = nearZ;
				cascade.mFarZ = farZ;

				cascade.mStaticMeshesBegin = mRenderQueue.mRenderData.mStaticMeshes.size();
				cascade.mAnimatedMeshesBegin = mRenderQueue.mRenderData.mAnimatedMeshes.size();

                const auto kdopIterator = dirLight.GetBoundingVolume(cascadeIndex);

                CullActors<decltype(staticActors)>(scene, mResourceManifest, mCullingStrategy, staticActors, parseMaterials, mRenderQueue.mRenderData, DirectionalLightCull, kdopIterator);
                CullActors<decltype(animatedActors)>(scene, mResourceManifest, mCullingStrategy, animatedActors, parseMaterials, mRenderQueue.mRenderData, DirectionalLightCull, kdopIterator);

				cascade.mStaticMeshesEnd = mRenderQueue.mRenderData.mStaticMeshes.size();
				cascade.mAnimatedMeshesEnd = mRenderQueue.mRenderData.mAnimatedMeshes.size();
            }
        }
    }

	void SceneParser::CopyBoneTransforms(const Scene& scene)
	{
		const AnimationUpdater& animUpdater = scene.GetAnimationUpdater();
		const BoneTransforms& bones = animUpdater.GetBonedata();
		mRenderQueue.mRenderData.mBones.insert(mRenderQueue.mRenderData.mBones.begin(), bones.begin(), bones.end());
	}

	void SceneParser::GetSkybox(const Scene& scene)
	{
		const SkyboxID skyboxID = scene.GetSkybox();
		DX11TextureID skyboxTexture = INVALID_DX11_TEXTURE_ID;
		if (skyboxID != INVALID_SKYBOX_ID)
		{
			const Skybox& skybox = mResourceManifest.GetSkybox(skyboxID);
		    skyboxTexture = skybox.GetSkyboxTexture();
		}

		mRenderQueue.mSkyboxTextureID = skyboxTexture;
	}

	void SceneParser::GetAmbientLight(const Scene& scene)
	{
		mRenderQueue.mAmbientLight = scene.GetAmbientLight();
	}

	void SceneParser::AddAABBDebugData(const Scene& scene)
	{
		mRenderQueue.mAABBRenderData.mCameraViewProjectionMatrix = mRenderQueue.mCamera.mCameraViewProjectionMatrix;

		const auto staticActors = scene.GetStaticActors();
		const auto animatedActors = scene.GetAnimatedActors();
		auto& aabbDataContainer = mRenderQueue.mAABBRenderData.mRenderableAABBs;
		const Mat4& viewProjTransform = mRenderQueue.mAABBRenderData.mCameraViewProjectionMatrix;

		CullAABB<decltype(staticActors)>(scene, mResourceManifest, staticActors, mCullingStrategy, aabbDataContainer, viewProjTransform);
		CullAABB<decltype(animatedActors)>(scene, mResourceManifest, animatedActors, mCullingStrategy, aabbDataContainer, viewProjTransform);
	}


    template <typename ACTOR_ITER_TYPE, typename VISIBLITY_FUNC, typename ...VISIBILITY_ARGS>
    void CullActors(const Scene& scene, const ResourceManifest& resManifest, const EngineSettings::CullingStrategy cullingStrat, const ACTOR_ITER_TYPE& actorIterator, const bool parseMaterials,
		RenderQueue::RenderData& renderData, const VISIBLITY_FUNC& testVisibilityFunc, VISIBILITY_ARGS&&... args)
    {
        const AnimationUpdater& animationUpdater = scene.GetAnimationUpdater();

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
                AddAllMeshes(actor, model, worldMatrix, parseMaterials, animationUpdater, resManifest, renderData);
        }
    }

	/*template <typename VISIBLITY_FUNC, typename ...VISIBILITY_ARGS>
	void CullTerrain(const Scene& scene, const ResourceManifest& resManifest, const EngineSettings::CullingStrategy cullingStrat, const ACTOR_ITER_TYPE& actorIterator,
		RenderQueue::RenderData& renderData, const VISIBLITY_FUNC& testVisibilityFunc, VISIBILITY_ARGS&&... args)
	{
		const auto visibilityResult = testVisibilityFunc(model, worldMatrix, std::forward<VISIBILITY_ARGS>(args)...);

		// TODO: extend with fine-grained culling, such as adding meshes per-node rather than per-model for AGGRESSIVE culling strategy,
		const bool addAllMeshes = DetermineIfAddAllMeshes(cullingStrat, visibilityResult);
		if (addAllMeshes)
			AddAllMeshes(actor, model, worldMatrix, parseMaterials, animationUpdater, resManifest, renderData);
	}*/


    template <>
    bool DetermineIfAddAllMeshes(const EngineSettings::CullingStrategy cullingStrat, const AABBIntersection aabbIntersection)
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
	void AddAllMeshes(const ACTOR_TYPE& actor, const Model& model, const Mat4& worldMatrix, const bool parseMaterials, const AnimationUpdater& animationUpdater, const ResourceManifest& resManifest,
		RenderQueue::RenderData& renderData)
	{
		for (const ModelNode& node : model.GetNodes())
		{
			if (node.GetNumMeshes() == 0)
				continue;

			for (const Mesh& mesh : node.GetMeshes())
			{
				const RenderableMaterial::Index materialID = ParseMaterial(actor, mesh, parseMaterials, resManifest, renderData.mMaterials);
				const float materialTilingFactor = actor.GetMaterialTilingFactor();
				const DX11MeshID meshID = mesh.GetMesh();
				const Mat4& localTransform = node.GetLocalTransform();

				AddMesh(actor, worldMatrix, localTransform, materialID, materialTilingFactor, meshID, animationUpdater, renderData);
			}
		}
	}
	

	template <>
	void AddMesh(const StaticActor& actor, const Mat4& worldTransform, const Mat4& localTransform, const RenderableMaterial::Index materialIndex, const float materialTilingFactor, const DX11MeshID mesh, const AnimationUpdater& animationUpdater, RenderQueue::RenderData& renderData)
	{
		const Mat4 localWorldTransform = worldTransform * localTransform;

		renderData.mStaticMeshes.emplace_back(localWorldTransform, materialIndex, materialTilingFactor, mesh);
	}

	template <>
	void AddMesh(const AnimatedActor& actor, const Mat4& worldTransform, const Mat4& localTransform, const RenderableMaterial::Index materialIndex, const float materialTilingFactor, const DX11MeshID mesh, const AnimationUpdater& animationUpdater, RenderQueue::RenderData& renderData)
	{
		const Mat4 localWorldTransform = worldTransform * localTransform;

		if (actor.IsPlaying())
		{
			const auto animInstanceID = actor.GetAnimationInstance();
			const auto& animBoneRange = animationUpdater.GetBoneRange(animInstanceID);

			// NOTE: might be a problem that all the node transforms up to the root bone is NOT added here!!
			renderData.mAnimatedMeshes.emplace_back(localWorldTransform, animBoneRange, materialIndex, materialTilingFactor, mesh);
		}
		else
			renderData.mStaticMeshes.emplace_back(localWorldTransform, materialIndex, materialTilingFactor, mesh);
	}


	template <typename ACTOR_TYPE>
	RenderableMaterial::Index ParseMaterial(const ACTOR_TYPE& actor, const Mesh& mesh, const bool parseMaterial, const ResourceManifest& resManifest, RenderableMaterial::ContainerType& materialContainer)
	{
		if (!parseMaterial)
			return RenderableMaterial::INVALID_INDEX;

		MaterialID materialID = mesh.GetDefaultMaterial();
		const MaterialID explicitActorMaterial = actor.GetMaterial();

		// an explicit material set on the actor will override the meshes default material
		if (explicitActorMaterial != INVALID_MATERIAL_ID)
			materialID = explicitActorMaterial;

		if (materialID == INVALID_MATERIAL_ID)
			return RenderableMaterial::INVALID_INDEX;

		const Material& material = resManifest.GetMaterial(materialID);
		const DX11TextureID diffuseTexture = material.GetDiffuseTexture();
		const DX11TextureID normalTexture = material.GetNormalTexture();
		const float specularFactor = material.GetSpecularFactor();

		materialContainer.emplace_back(diffuseTexture, normalTexture, specularFactor);
		const RenderableMaterial::Index index = materialContainer.size() - 1;

		return index;
	}

	template <typename ACTOR_ITER_TYPE>
	void CullAABB(const Scene& scene, const ResourceManifest& resManifest, const ACTOR_ITER_TYPE& actorIterator, const EngineSettings::CullingStrategy cullingStrat, AABBRenderData::RenderableAABBsContainer& aabbDataContainer,
		const Mat4& viewProjTransform)
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

			const auto visibilityResult = FrustumCull(model, worldMatrix, viewProjTransform);

			// TODO: extend with fine-grained culling, such as adding meshes per-node rather than per-model for AGGRESSIVE culling strategy,
			const bool addAllMeshes = DetermineIfAddAllMeshes(cullingStrat, visibilityResult);
			if (addAllMeshes)
			{
				for (const ModelNode& node : model.GetNodes())
				{
					if (node.GetNumMeshes() == 0)
						continue;

					for (const Mesh& mesh : node.GetMeshes())
					{
						const DX11MeshID meshID = mesh.GetMesh();
						aabbDataContainer.emplace_back(worldMatrix, meshID);
					}
				}
			}
		}
	}
}