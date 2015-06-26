#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineDefs.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Math/Intersection.h"

#include "boost/functional/hash.hpp"
#include <algorithm>
#include <functional>

namespace JonsEngine
{
    void AddMesh(std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const IDMap<Mat4>::ItemID localTransformID, const IDMap<Mat4>::ItemID worldTransformID)
    {
        resultMeshes.emplace_back(mesh.mMeshID, localTransformID, worldTransformID, mesh.GetMaterial()->mDiffuseTexture, mesh.GetMaterial()->mNormalTexture, mesh.GetMaterial()->mSpecularFactor, mesh.GetTextureTilingFactor());
    }

    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const IDMap<Mat4>::ItemID localTransformID, const IDMap<Mat4>::ItemID worldTransformID)
    {
        resultMeshes.emplace_back(mesh.mMeshID, localTransformID, worldTransformID);
    }

    template <typename T>
    void AddAllMeshes(std::vector<T>& resultMeshes, ModelNode& node, const IDMap<Mat4>::ItemID worldTransformID)
    {
		for (const Mesh& mesh : node.GetMeshes())
            AddMesh(resultMeshes, mesh, node.GetLocalTransformID(), worldTransformID);

		for (ModelNode& node : node.GetChildNodes())
            AddAllMeshes(resultMeshes, node, worldTransformID);
    }

    template <typename T>
    void CullMeshesFrustrum(std::vector<T>& resultMeshes, ModelNode& node, const Mat4& wvpMatrix, const IDMap<Mat4>::ItemID worldMatrixID)
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
                        AddMesh(resultMeshes, mesh, node.GetLocalTransformID(), worldMatrixID);
				}

                // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
				for (ModelNode& node : node.GetChildNodes())
                    CullMeshesFrustrum(resultMeshes, node, wvpMatrix, worldMatrixID);

				break;
			}

            case AABBIntersection::Inside:
			{
                AddAllMeshes(resultMeshes, node, worldMatrixID);
				break;
			}

            case AABBIntersection::Outside:
			default:
				break;
		}
    }

    void CullMeshesAABB(std::vector<RenderableMesh>& resultMeshes, ModelNode& node, const AABB& aabb, const IDMap<Mat4>::ItemID worldTransformID)
    {
        // test node AABB
        AABBIntersection nodeAABBIntersection = Intersection(node.mLocalAABB, aabb);
        switch (nodeAABBIntersection)
        {
            // if partially inside, recursively test all meshes and child nodes
            case AABBIntersection::Partial:
            {
                AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

                for (const Mesh& mesh : node.GetMeshes())
                {
                    meshAABBIntersection = Intersection(mesh.mLocalAABB, aabb);
                    if (meshAABBIntersection == AABBIntersection::Outside)
                        continue;

                    if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                        AddMesh(resultMeshes, mesh, node.GetLocalTransformID(), worldTransformID);
                }

                // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
                for (ModelNode& node : node.GetChildNodes())
                    CullMeshesAABB(resultMeshes, node, aabb, worldTransformID);

                break;
            }

            case AABBIntersection::Inside:
        {
            AddAllMeshes(resultMeshes, node, worldTransformID);
            break;
        }

        case AABBIntersection::Outside:
        default:
            break;
        }
    }

    void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, ModelNode& node, const Mat4& worldMatrix, const IDMap<Mat4>::ItemID worldTransformID, const Vec3& sphereCentre, const float sphereRadius)
    {
		const AABB worldAABB = worldMatrix * node.mLocalAABB;

        // test node frustrum
        AABBIntersection nodeAABBIntersection = Intersection(worldAABB, sphereCentre, sphereRadius);
        switch (nodeAABBIntersection)
        {
			case AABBIntersection::Partial:
			{
                AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

				for (const Mesh& mesh : node.GetMeshes())
				{
					const AABB meshAABB = worldMatrix * node.mLocalAABB;

                    meshAABBIntersection = Intersection(meshAABB, sphereCentre, sphereRadius);
                    if (meshAABBIntersection == AABBIntersection::Outside)
						continue;

                    if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                        AddMesh(resultMeshes, mesh, node.GetLocalTransformID(), worldTransformID);
				}

				// each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
				for (ModelNode& node : node.GetChildNodes())
                    CullMeshesSphere(resultMeshes, node, worldMatrix, worldTransformID, sphereCentre, sphereRadius);

				break;
			}

            case AABBIntersection::Inside:
			{
                AddAllMeshes(resultMeshes, node, worldTransformID);
				break;
			}

            case AABBIntersection::Outside:
			default:
				break;
        }
    }


    Scene::Scene(const std::string& sceneName, const IDMap<Mat4>& modelTransformCache) :
        mName(sceneName), mHashedID(boost::hash_value(sceneName)), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()),
        // TODO: lambda expression adds a layer of indirection, but I'm not sure how the bind syntax would look like
        mRootNode("Root", mTransformCache.GetStorage(), [&](SceneNode* node) { mDirtySceneNodes.push_back(node); }), mAmbientLight(0.2f), mSkybox(nullptr), mRenderQueue(modelTransformCache, mTransformCache.GetStorage())
    {
    }

    Scene::~Scene()
    {
    }


    bool Scene::operator==(const Scene& s1)
    {
        return mHashedID == s1.mHashedID;
    }

    bool Scene::operator==(const std::string& sceneName)
    {
        return mHashedID == boost::hash_value(sceneName);
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
            const IDMap<Mat4>::ItemID worldMatrixID = actor->mSceneNode->GetWorldTransformID();
            const Mat4 wvpMatrix = mRenderQueue.mCamera.mCameraViewProjectionMatrix * worldMatrix;

            CullMeshesFrustrum<RenderableModel>(mRenderQueue.mCamera.mModels, actor->mModel->GetRootNode(), wvpMatrix, worldMatrixID);
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
                const IDMap<Mat4>::ItemID worldTransformID = actor->mSceneNode->GetWorldTransformID();
                CullMeshesSphere(renderablePointLight.mMeshes, actor->mModel->GetRootNode(), actorWorldMatrix, worldTransformID, lightPosition, pointLight.mLightRadius);
            }
        }

        // dir lights
        for (DirectionalLightPtr& dirLight : mDirectionalLights)
        {
            mRenderQueue.mDirectionalLights.emplace_back(dirLight->mLightColor, glm::normalize(dirLight->mLightDirection), dirLight->mNumShadowmapCascades);
            RenderableDirLight& renderableDirLight = mRenderQueue.mDirectionalLights.back();

            dirLight->UpdateCascadesBoundingVolume(mRenderQueue.mCamera.mCameraViewMatrix, fov, aspectRatio, minDepth, maxDepth);
            for (uint32_t cascadeIndex = 0; cascadeIndex < dirLight->mNumShadowmapCascades; ++cascadeIndex)
            {
                float nearZ = 0.0f, farZ = 0.0f;
                dirLight->GetSplitDistance(cascadeIndex, nearZ, farZ);

                auto kdopIterator = dirLight->GetBoundingVolume(cascadeIndex);
                for (const ActorPtr& actor : mActors)
                {
                    if (!actor->mSceneNode)
                        continue;

                    const Mat4& worldMatrix = actor->mSceneNode->GetWorldTransform();
                    const IDMap<Mat4>::ItemID worldTransformID = actor->mSceneNode->GetWorldTransformID();
                    const Mat4 localWorldMatrix = worldMatrix * actor->mModel->GetRootNode().GetLocalTransform();
                    const AABB worldAABB = localWorldMatrix * actor->mModel->GetRootNode().mLocalAABB;

                    const auto aabbIntersection = Intersection(worldAABB, kdopIterator);
                    if (aabbIntersection == AABBIntersection::Inside || aabbIntersection == AABBIntersection::Partial)
                        AddAllMeshes(renderableDirLight.mMeshes, actor->mModel->GetRootNode(), worldTransformID);
                }

                renderableDirLight.mCascadeSplits.emplace_back(nearZ, farZ, renderableDirLight.mMeshes.size());
            }
        }

        // misc
        mRenderQueue.mAmbientLight = mAmbientLight;
        mRenderQueue.mSkyboxTextureID = mSkybox.get() ? mSkybox->mSkyboxTexture : INVALID_TEXTURE_ID;

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
    

    DirectionalLight* Scene::CreateDirectionalLight(const std::string& lightName)
    {
        const uint32_t defaultNumCascades = 4;

        mDirectionalLights.emplace_back(mMemoryAllocator.AllocateObject<DirectionalLight>(lightName, defaultNumCascades), std::bind(&HeapAllocator::DeallocateObject<DirectionalLight>, &mMemoryAllocator, std::placeholders::_1));

        return mDirectionalLights.back().get();
    }

    DirectionalLight* Scene::CreateDirectionalLight(const std::string& lightName, const uint32_t numShadowmapCascades)
    {
        mDirectionalLights.emplace_back(mMemoryAllocator.AllocateObject<DirectionalLight>(lightName, numShadowmapCascades), std::bind(&HeapAllocator::DeallocateObject<DirectionalLight>, &mMemoryAllocator, std::placeholders::_1));

        return mDirectionalLights.back().get();
    }
    
    void Scene::DeleteDirectionalLight(const DirectionalLight* dirLight)
    {
        auto iter = std::find_if(mDirectionalLights.begin(), mDirectionalLights.end(), [dirLight](const DirectionalLightPtr& storedLight) { return storedLight.get() == dirLight; });

        if (iter != mDirectionalLights.end())
            mDirectionalLights.erase(iter);
    }
    
    DirectionalLight* Scene::GetDirectionalLight(const std::string& lightName)
    {
        auto iter = std::find_if(mDirectionalLights.begin(), mDirectionalLights.end(), [lightName](const DirectionalLightPtr& storedLight) { return *storedLight == lightName; });
        if (iter == mDirectionalLights.end())
            return nullptr;

        return iter->get();
    }

    const std::vector<DirectionalLightPtr>& Scene::GetDirectionalLights() const
    {
        return mDirectionalLights;
    }


	void Scene::SetAmbientLight(const Vec4& ambientLight)
    {
        mAmbientLight = ambientLight;
    }

    const Vec4& Scene::GetAmbientLight() const
    {
        return mAmbientLight;
    }


    void Scene::SetSkybox(const SkyboxPtr skybox)
    {
        mSkybox = skybox;
    }

    SkyboxPtr Scene::GetSkybox() const
    {
        return mSkybox;
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