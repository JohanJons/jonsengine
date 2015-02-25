#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineDefs.h"
#include "include/Core/Utils/Math.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/functional/hash.hpp"
#include <algorithm>

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
            // TODO: if partially inside, recursively test all meshes and child nodes
			// TODO: check for INSIDE
			case AABBIntersection::AABB_INTERSECTION_PARTIAL:
			{
				AddAllMeshes(resultMeshes, node, Mat4(1.0f), worldMatrix);
				break;
			}

			case AABBIntersection::AABB_INTERSECTION_OUTSIDE:
			default:
				break;
        }
    }


    Scene::Scene(const std::string& sceneName) : mName(sceneName), mHashedID(boost::hash_value(sceneName)), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mRootNode("Root"), mAmbientLight(0.2f)
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
        mRenderQueue.Clear();

        const Camera& sceneCamera = GetSceneCamera();

        // camera
        mRenderQueue.mCamera.mFOV = sceneCamera.GetFOV();
        mRenderQueue.mCamera.mCameraPosition = sceneCamera.Position();
        mRenderQueue.mCamera.mCameraProjectionMatrix = PerspectiveMatrixFov(mRenderQueue.mCamera.mFOV, windowWidth / static_cast<float>(windowHeight), zNear, zFar);
        mRenderQueue.mCamera.mCameraViewMatrix = sceneCamera.GetCameraTransform();
        const Mat4 cameraViewProjMatrix = mRenderQueue.mCamera.mCameraProjectionMatrix * mRenderQueue.mCamera.mCameraViewMatrix;

		for (const ActorPtr& actor : mActors)
		{
			if (!actor->mSceneNode)
				continue;

			const Mat4& worldMatrix = actor->mSceneNode->GetNodeTransform();
            const Mat4 wvpMatrix = cameraViewProjMatrix * worldMatrix;

            CullMeshesFrustrum<RenderableModel>(mRenderQueue.mCamera.mModels, actor->mModel->GetRootNode(), wvpMatrix, worldMatrix);
		}

        // point lights
        for (const PointLightPtr& pointLight : mPointLights)
        {
			if (!pointLight->mSceneNode)
				continue;

			const Vec3 lightPosition = pointLight->mSceneNode->Position();
            const Mat4 worldMatrix = pointLight->mSceneNode->GetNodeTransform();

            // scaled WVP is used for stencil op
            // TODO: move to renderer
            const Mat4 scaledWorldMatrix = glm::scale(worldMatrix, Vec3(pointLight->mMaxDistance));
            const Mat4 scaledWVPMatrix = cameraViewProjMatrix * scaledWorldMatrix;

			mRenderQueue.mPointLights.emplace_back(scaledWVPMatrix, pointLight->mLightColor, lightPosition, pointLight->mLightIntensity, pointLight->mMaxDistance);
            RenderablePointLight& renderablePointLight = mRenderQueue.mPointLights.back();

			//  cull meshes for each face
			for (const ActorPtr& actor : mActors)
			{
				if (!actor->mSceneNode)
					continue;

				const Mat4& actorWorldMatrix = actor->mSceneNode->GetNodeTransform();
				CullMeshesSphere(renderablePointLight.mMeshes, actor->mModel->GetRootNode(), actorWorldMatrix, lightPosition, pointLight->mMaxDistance);
			}
        }

        // dir lights
        for (const DirectionalLightPtr& dirLight : mDirectionalLights)
        {
            // TODO
            mRenderQueue.mDirectionalLights.emplace_back(dirLight->mLightColor, dirLight->mLightDirection);
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


    PointLight* Scene::CreatePointLight(const std::string& lightName, const SceneNodePtr node)
    {
        mPointLights.emplace_back(mMemoryAllocator.AllocateObject<PointLight>(lightName, node), std::bind(&HeapAllocator::DeallocateObject<PointLight>, &mMemoryAllocator, std::placeholders::_1));

        return mPointLights.back().get();
    }
    
    void Scene::DeletePointLight(const PointLight* pointLight)
    {
        auto iter = std::find_if(mPointLights.begin(), mPointLights.end(), [pointLight](const PointLightPtr& storedLight) { return storedLight.get() == pointLight; });

        if (iter != mPointLights.end())
            mPointLights.erase(iter);
    }
    
    PointLight* Scene::GetPointLight(const std::string& lightName)
    {
		auto iter = std::find_if(mPointLights.begin(), mPointLights.end(), [lightName](const PointLightPtr& storedLight) { return *storedLight == lightName; });
        if (iter == mPointLights.end())
            return nullptr;

        return iter->get();
    }
   
    const std::vector<PointLightPtr>& Scene::GetPointLights() const
    {
        return mPointLights;
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
}