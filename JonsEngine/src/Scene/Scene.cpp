#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineDefs.h"
#include "include/Core/Utils/Math.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/functional/hash.hpp"
#include <algorithm>
#include <functional>

namespace JonsEngine
{
    void AddMesh(std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
		resultMeshes.emplace_back(mesh.mMeshID, wvpMatrix, worldMatrix, mesh.GetMaterial()->mDiffuseTexture, mesh.GetMaterial()->mNormalTexture, mesh.GetMaterial()->mSpecularFactor, mesh.GetTextureTilingFactor());
    }

    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        resultMeshes.emplace_back(mesh.mMeshID, wvpMatrix, worldMatrix);
    }

    template <typename T>
    void AddAllMeshes(std::vector<T>& resultMeshes, ModelNode& node, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.mTransform;

		for (const Mesh& mesh : node.GetMeshes())
            AddMesh(resultMeshes, mesh, localWVPMatrix, worldMatrix * node.mTransform);

		for (ModelNode& node : node.GetChildNodes())
            AddAllMeshes(resultMeshes, node, wvpMatrix, worldMatrix);
    }

    template <typename T>
    void CullMeshesFrustrum(std::vector<T>& resultMeshes, ModelNode& node, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.mTransform;

        // test node frustrum
		// TODO: does this cull objects behind frustrum that still intersects some planes?
        AABBIntersection nodeAABBIntersection = IsAABBInFrustum(node.mAABB, localWVPMatrix);
        switch (nodeAABBIntersection)
        {
			// if partially inside, recursively test all meshes and child nodes
			case AABBIntersection::AABB_INTERSECTION_PARTIAL:
			{
				AABBIntersection meshAABBIntersection(AABBIntersection::AABB_INTERSECTION_INSIDE);

				for (const Mesh& mesh : node.GetMeshes())
				{
                    meshAABBIntersection = IsAABBInFrustum(mesh.mAABB, localWVPMatrix);
					if (meshAABBIntersection == AABBIntersection::AABB_INTERSECTION_OUTSIDE)
						continue;

					if (meshAABBIntersection == AABBIntersection::AABB_INTERSECTION_INSIDE || meshAABBIntersection == AABBIntersection::AABB_INTERSECTION_PARTIAL)
                        AddMesh(resultMeshes, mesh, localWVPMatrix, worldMatrix * node.mTransform);
				}

                // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
				for (ModelNode& node : node.GetChildNodes())
                    CullMeshesFrustrum(resultMeshes, node, wvpMatrix, worldMatrix);

				break;
			}

			case AABBIntersection::AABB_INTERSECTION_INSIDE:
			{
                AddAllMeshes(resultMeshes, node, wvpMatrix, worldMatrix);
				break;
			}

			case AABBIntersection::AABB_INTERSECTION_OUTSIDE:
			default:
				break;
		}
    }

	void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius)
    {
		const AABB worldAABB = worldMatrix * node.mAABB;

        // test node frustrum
		AABBIntersection nodeAABBIntersection = IsAABBInSphere(worldAABB, sphereCentre, sphereRadius);
        switch (nodeAABBIntersection)
        {
			case AABBIntersection::AABB_INTERSECTION_PARTIAL:
			{
				AABBIntersection meshAABBIntersection(AABBIntersection::AABB_INTERSECTION_INSIDE);

				for (const Mesh& mesh : node.GetMeshes())
				{
					const AABB meshAABB = worldMatrix * node.mAABB;

					meshAABBIntersection = IsAABBInSphere(meshAABB, sphereCentre, sphereRadius);
					if (meshAABBIntersection == AABBIntersection::AABB_INTERSECTION_OUTSIDE)
						continue;

					if (meshAABBIntersection == AABBIntersection::AABB_INTERSECTION_INSIDE || meshAABBIntersection == AABBIntersection::AABB_INTERSECTION_PARTIAL)
						AddMesh(resultMeshes, mesh, Mat4(1.0f), worldMatrix * node.mTransform);
				}

				// each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
				for (ModelNode& node : node.GetChildNodes())
					CullMeshesSphere(resultMeshes, node, worldMatrix, sphereCentre, sphereRadius);

				break;
			}

			case AABBIntersection::AABB_INTERSECTION_INSIDE:
			{
				AddAllMeshes(resultMeshes, node, Mat4(1.0f), worldMatrix);
				break;
			}

			case AABBIntersection::AABB_INTERSECTION_OUTSIDE:
			default:
				break;
        }
    }

    void AddAll(std::vector<RenderableMesh>& resultMeshes, ModelNode& node, const Mat4& worldMatrix)
    {
        AddAllMeshes(resultMeshes, node, Mat4(1.0f), worldMatrix);
    }


    Scene::Scene(const std::string& sceneName) :
        mName(sceneName), mHashedID(boost::hash_value(sceneName)), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()),
        // TODO: lambda expression adds a layer of indirection, but I'm not sure how the bind syntax would look like
        mRootNode("Root", [&](SceneNode* node) { mDirtySceneNodes.push_back(node); }), mAmbientLight(0.2f)
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


    const RenderQueue& Scene::GetRenderQueue(const uint32_t windowWidth, const uint32_t windowHeight, const float zNear, const float zFar)
    {
        UpdateDirtyObjects();

        mRenderQueue.Clear();

        const Camera& sceneCamera = GetSceneCamera();

        // camera
        mRenderQueue.mCamera.mFOV = sceneCamera.GetFOV();
        mRenderQueue.mCamera.mCameraPosition = sceneCamera.Position();
        mRenderQueue.mCamera.mCameraViewMatrix = sceneCamera.GetCameraTransform();
        mRenderQueue.mCamera.mCameraProjectionMatrix = PerspectiveMatrixFov(mRenderQueue.mCamera.mFOV, windowWidth / static_cast<float>(windowHeight), zNear, zFar);
        mRenderQueue.mCamera.mCameraViewProjectionMatrix = mRenderQueue.mCamera.mCameraProjectionMatrix * mRenderQueue.mCamera.mCameraViewMatrix;

		for (const ActorPtr& actor : mActors)
		{
			if (!actor->mSceneNode)
				continue;

            const Mat4& worldMatrix = actor->mSceneNode->GetWorldMatrix();
            const Mat4 wvpMatrix = mRenderQueue.mCamera.mCameraViewProjectionMatrix * worldMatrix;

            CullMeshesFrustrum<RenderableModel>(mRenderQueue.mCamera.mModels, actor->mModel->GetRootNode(), wvpMatrix, worldMatrix);
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

                const Mat4& actorWorldMatrix = actor->mSceneNode->GetWorldMatrix();
                CullMeshesSphere(renderablePointLight.mMeshes, actor->mModel->GetRootNode(), actorWorldMatrix, lightPosition, pointLight.mLightRadius);
            }
        }

        // dir lights
        for (const DirectionalLightPtr& dirLight : mDirectionalLights)
        {
            // TODO
            mRenderQueue.mDirectionalLights.emplace_back(dirLight->mLightColor, dirLight->mLightDirection);
            RenderableDirLight& dirLight = mRenderQueue.mDirectionalLights.back();

            for (const ActorPtr& actor : mActors)
            {
                if (!actor->mSceneNode)
                    continue;

                const Mat4& actorWorldMatrix = actor->mSceneNode->GetWorldMatrix();
                AddAll(dirLight.mMeshes, actor->mModel->GetRootNode(), actorWorldMatrix);
            }
        }

        // misc
        mRenderQueue.mAmbientLight = mAmbientLight;

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
        mDirectionalLights.emplace_back(mMemoryAllocator.AllocateObject<DirectionalLight>(lightName), std::bind(&HeapAllocator::DeallocateObject<DirectionalLight>, &mMemoryAllocator, std::placeholders::_1));

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