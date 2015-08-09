#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Math/Intersection.h"

#include <algorithm>
#include <functional>

namespace JonsEngine
{
    void AddMesh(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix)
    {
        resultMeshes.emplace_back(mesh.mMeshID, localWorldMatrix, mesh.GetMaterial()->mDiffuseTexture, mesh.GetMaterial()->mNormalTexture, mesh.GetMaterial()->mSpecularFactor, mesh.GetTextureTilingFactor());
    }

    void AddMesh(const ResourceManifest& resourceManifest, std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix)
    {
        resultMeshes.emplace_back(mesh.mMeshID, localWorldMatrix);
    }

    template <typename T>
    void AddAllMeshes(const ResourceManifest& resourceManifest, std::vector<T>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix)
    {
		for (const Mesh& mesh : node.GetMeshes())
            AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * node.mLocalTransform);

		for (ModelNode& node : node.GetChildNodes())
            AddAllMeshes(resourceManifest, resultMeshes, node, worldMatrix);
    }

    template <typename T>
    void CullMeshesFrustrum(const ResourceManifest& resourceManifest, std::vector<T>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Mat4& wvpMatrix)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.GetLocalTransform();

        // test node frustrum
		// TODO: does this cull objects behind frustrum that still intersects some planes?
        AABBIntersection nodeAABBIntersection = Intersection(node.mLocalAABB, localWVPMatrix);
        switch (nodeAABBIntersection)
        {
			// if partially inside, recursively test all meshes and child nodes
			case AABBIntersection::Partial:
			{
                AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

				for (const Mesh& mesh : node.GetMeshes())
				{
                    meshAABBIntersection = Intersection(mesh.mLocalAABB, localWVPMatrix);
                    if (meshAABBIntersection == AABBIntersection::Outside)
						continue;

                    if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                        AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * node.mLocalTransform);
				}

                // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
				for (const ModelNode& node : node.GetChildNodes())
                    CullMeshesFrustrum(resourceManifest, resultMeshes, node, wvpMatrix, worldMatrixID);

				break;
			}

            case AABBIntersection::Inside:
			{
                AddAllMeshes(resourceManifest, resultMeshes, node, worldMatrixID);
				break;
			}

            case AABBIntersection::Outside:
			default:
				break;
		}
    }

    void CullMeshesAABB(const ResourceManifest& resourceManifest, std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const AABB& aabb, const Mat4& worldMatrix)
    {
        // test node AABB
        AABBIntersection nodeAABBIntersection = Intersection(node.mLocalAABB, aabb);
        switch (nodeAABBIntersection)
        {
            // if partially inside, recursively test all meshes and child nodes
            case AABBIntersection::Partial:
            {
                AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

                for (const Mesh& mesh : node.mMeshes)
                {
                    meshAABBIntersection = Intersection(mesh.mLocalAABB, aabb);
                    if (meshAABBIntersection == AABBIntersection::Outside)
                        continue;

                    if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                        AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * node.mLocalTransform);
                }

                // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
                for (const ModelNode& node : node.mChildren)
                    CullMeshesAABB(resourceManifest, resultMeshes, node, aabb, worldMatrix);

                break;
            }

            case AABBIntersection::Inside:
        {
            AddAllMeshes(resourceManifest, resultMeshes, node, worldMatrix);
            break;
        }

        case AABBIntersection::Outside:
        default:
            break;
        }
    }

    void CullMeshesSphere(const ResourceManifest& resourceManifest, std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius)
    {
		const AABB worldAABB = worldMatrix * node.mLocalAABB;

        // test node frustrum
        AABBIntersection nodeAABBIntersection = Intersection(worldAABB, sphereCentre, sphereRadius);
        switch (nodeAABBIntersection)
        {
			case AABBIntersection::Partial:
			{
                AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

				for (const Mesh& mesh : node.mMeshes)
				{
					const AABB meshAABB = worldMatrix * node.mLocalAABB;

                    meshAABBIntersection = Intersection(meshAABB, sphereCentre, sphereRadius);
                    if (meshAABBIntersection == AABBIntersection::Outside)
						continue;

                    if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                        AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * node.mLocalTransform);
				}

				// each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
				for (const ModelNode& node : node.mChildren)
                    CullMeshesSphere(resourceManifest, resultMeshes, node, worldMatrix, sphereCentre, sphereRadius);

				break;
			}

            case AABBIntersection::Inside:
			{
                AddAllMeshes(resourceManifest, resultMeshes, node, worldMatrix);
				break;
			}

            case AABBIntersection::Outside:
			default:
				break;
        }
    }


    Scene::Scene(const std::string& sceneName, const ResourceManifest& resourceManifest) :
        mName(sceneName), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mResourceManifest(resourceManifest),
        // TODO: lambda expression adds a layer of indirection, but I'm not sure how the bind syntax would look like
        mRootNode("Root", mTransformCache.GetStorage(), [&](SceneNode* node) { mDirtySceneNodes.push_back(node); }), mAmbientLight(0.2f), mSkyboxID(IDMap<Skybox>::INVALID_ITEM_ID)
    {
    }

    Scene::~Scene()
    {
    }


    const RenderQueue& Scene::GetRenderQueue(const Mat4& cameraProjectionMatrix, const float fov, const float aspectRatio, const float minDepth, const float maxDepth)
    {
        UpdateDirtyObjects();

        mRenderQueue.Clear();

        const Camera& sceneCamera = GetSceneCamera();

        // camera
        mRenderQueue.mCamera.mFOV = fov;
        mRenderQueue.mCamera.mCameraPosition = sceneCamera.Position();
        mRenderQueue.mCamera.mCameraViewMatrix = sceneCamera.GetCameraTransform();
        mRenderQueue.mCamera.mCameraProjectionMatrix = cameraProjectionMatrix;
        mRenderQueue.mCamera.mCameraViewProjectionMatrix = mRenderQueue.mCamera.mCameraProjectionMatrix * mRenderQueue.mCamera.mCameraViewMatrix;

		for (const ActorPtr& actor : mActors)
		{
			if (!actor->mSceneNode)
				continue;

            const Mat4& worldMatrix = actor->mSceneNode->GetWorldTransform();
            const Mat4 wvpMatrix = mRenderQueue.mCamera.mCameraViewProjectionMatrix * worldMatrix;

            CullMeshesFrustrum<RenderableModel>(mResourceManifest, mRenderQueue.mCamera.mModels, actor->mModel->GetRootNode(), worldMatrix, wvpMatrix);
		}

        // point lights
        for (const PointLight& pointLight : mPointLights)
        {
            if (!pointLight.mSceneNode)
                continue;

            const Vec3 lightPosition = pointLight.mSceneNode->Position();

            mRenderQueue.mPointLights.emplace_back(pointLight.mLightColor, lightPosition, pointLight.mLightIntensity, pointLight.mLightRadius);
            RenderablePointLight& renderablePointLight = mRenderQueue.mPointLights.back();

            //  cull meshes for each face
            for (const ActorPtr& actor : mActors)
            {
                if (!actor->mSceneNode)
                    continue;

                const Mat4& actorWorldMatrix = actor->mSceneNode->GetWorldTransform();
                CullMeshesSphere(mResourceManifest, renderablePointLight.mMeshes, actor->mModel->GetRootNode(), actorWorldMatrix, lightPosition, pointLight.mLightRadius);
            }
        }

        // dir lights
        for (DirectionalLight& dirLight : mDirectionalLights)
        {
            mRenderQueue.mDirectionalLights.emplace_back(dirLight.mLightColor, glm::normalize(dirLight.mLightDirection), dirLight.mNumShadowmapCascades);
            RenderableDirLight& renderableDirLight = mRenderQueue.mDirectionalLights.back();

            dirLight.UpdateCascadesBoundingVolume(mRenderQueue.mCamera.mCameraViewMatrix, fov, aspectRatio, minDepth, maxDepth);
            for (uint32_t cascadeIndex = 0; cascadeIndex < dirLight.mNumShadowmapCascades; ++cascadeIndex)
            {
                float nearZ = 0.0f, farZ = 0.0f;
                dirLight.GetSplitDistance(cascadeIndex, nearZ, farZ);

                auto kdopIterator = dirLight.GetBoundingVolume(cascadeIndex);
                for (const ActorPtr& actor : mActors)
                {
                    if (!actor->mSceneNode)
                        continue;

                    const Mat4& worldMatrix = actor->mSceneNode->GetWorldTransform();
                    const Mat4 localWorldMatrix = worldMatrix * actor->mModel->GetRootNode().mLocalTransform;
                    const AABB worldAABB = localWorldMatrix * actor->mModel->GetRootNode().mLocalAABB;

                    const auto aabbIntersection = Intersection(worldAABB, kdopIterator);
                    if (aabbIntersection == AABBIntersection::Inside || aabbIntersection == AABBIntersection::Partial)
                        AddAllMeshes(mResourceManifest, renderableDirLight.mMeshes, actor->mModel->GetRootNode(), worldMatrix);
                }

                renderableDirLight.mCascadeSplits.emplace_back(nearZ, farZ, renderableDirLight.mMeshes.size());
            }
        }

        // misc
        mRenderQueue.mAmbientLight = mAmbientLight;

        const Skybox& skybox = mResourceManifest.GetSkybox(mSkyboxID);
        mRenderQueue.mSkyboxTextureID = skybox.mSkyboxTexture;

        return mRenderQueue;
    }


    Actor* Scene::CreateActor(const std::string& actorName, const ModelPtr model, const SceneNodePtr node)
    {
        mActors.emplace_back(mMemoryAllocator.AllocateObject<Actor>(actorName, model, node), std::bind(&HeapAllocator::DeallocateObject<Actor>, &mMemoryAllocator, std::placeholders::_1));

        return mActors.back().get();
    }

    void Scene::DeleteActor(Actor* actor)
    {
        auto iter = std::find_if(mActors.begin(), mActors.end(), [actor](const ActorPtr& storedActor) { return storedActor.get() == actor; });

        if (iter != mActors.end())
            mActors.erase(iter);
    }

    Actor* Scene::GetActor(const std::string& actorName)
    {
        auto iter = std::find_if(mActors.begin(), mActors.end(), [actorName](const ActorPtr& storedActor) { return *storedActor == actorName; });
        if (iter == mActors.end())
            return nullptr;

        return iter->get();
    }

	const std::vector<ActorPtr>& Scene::GetActors() const
	{
		return mActors;
	}


	PointLightID Scene::CreatePointLight(const std::string& lightName, SceneNodePtr node)
    {
        return mPointLights.AddItem(lightName, node);
    }
    
	void Scene::DeletePointLight(const PointLightID pointLightID)
    {
		mPointLights.MarkAsFree(pointLightID);
    }
    
	PointLight& Scene::GetPointLight(const PointLightID pointLightID)
    {
		return mPointLights.GetItem(pointLightID);
    }
    

    DirectionalLightID Scene::CreateDirectionalLight(const std::string& lightName)
    {
        const uint32_t defaultNumCascades = 4;

        return mDirectionalLights.AddItem(lightName, defaultNumCascades);
    }

    DirectionalLightID Scene::CreateDirectionalLight(const std::string& lightName, const uint32_t numShadowmapCascades)
    {
        return mDirectionalLights.AddItem(lightName, numShadowmapCascades);
    }

    void Scene::DeleteDirectionalLight(const DirectionalLightID dirLight)
    {
        mDirectionalLights.MarkAsFree(dirLight);
    }

    DirectionalLight& Scene::GetDirectionalLight(const DirectionalLightID dirLightID)
    {
        return mDirectionalLights.GetItem(dirLightID);
    }


	void Scene::SetAmbientLight(const Vec4& ambientLight)
    {
        mAmbientLight = ambientLight;
    }

    const Vec4& Scene::GetAmbientLight() const
    {
        return mAmbientLight;
    }


    void Scene::SetSkybox(const SkyboxID skybox)
    {
        mSkyboxID = skybox;
    }

    SkyboxID Scene::GetSkybox() const
    {
        return mSkyboxID;
    }


	Camera& Scene::GetSceneCamera()
	{ 
		return mSceneCamera;
	}

    SceneNode& Scene::GetRootNode()
	{ 
		return mRootNode;
	}


    void Scene::UpdateDirtyObjects()
    {
        // flag all objects using dirty scene nodes as dirty as well
        for (SceneNode* node : mDirtySceneNodes)
        {
            // all children needs to be updated
            // TODO: sort based on hierarchy to avoid unnecessary operations if children present in mDirtySceneNodes aswell
            node->UpdateWorldMatrix();
        }

        mDirtySceneNodes.clear();
    }
}