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
    /*enum LSM_FRUSTUM_PLANES {
        LSM_FP_LEFT,
        LSM_FP_RIGHT,
        LSM_FP_TOP,
        LSM_FP_BOTTOM,
        LSM_FP_NEAR,
        LSM_FP_FAR,
        LSM_FP_TOTAL
    };

    enum LSM_FRUSTUM_POINTS {
        LSM_FP_NEAR_BOTTOM_LEFT,
        LSM_FP_NEAR_TOP_LEFT,
        LSM_FP_NEAR_TOP_RIGHT,
        LSM_FP_NEAR_BOTTOM_RIGHT,
        LSM_FP_FAR_BOTTOM_LEFT,
        LSM_FP_FAR_TOP_LEFT,
        LSM_FP_FAR_TOP_RIGHT,
        LSM_FP_FAR_BOTTOM_RIGHT,
    };

    void GetNeighbors(uint32_t index, LSM_FRUSTUM_PLANES _fpRet[4]) {
        static const LSM_FRUSTUM_PLANES fpTable[LSM_FP_TOTAL][4] = {
            {	// LSM_FP_LEFT
                LSM_FP_TOP,
                LSM_FP_BOTTOM,
                LSM_FP_NEAR,
                LSM_FP_FAR
            },
            {	// LSM_FP_RIGHT
                LSM_FP_TOP,
                LSM_FP_BOTTOM,
                LSM_FP_NEAR,
                LSM_FP_FAR
            },
            {	// LSM_FP_TOP
                LSM_FP_LEFT,
                LSM_FP_RIGHT,
                LSM_FP_NEAR,
                LSM_FP_FAR
            },
            {	// LSM_FP_BOTTOM
                LSM_FP_LEFT,
                LSM_FP_RIGHT,
                LSM_FP_NEAR,
                LSM_FP_FAR
            },
            {	// LSM_FP_NEAR
                LSM_FP_LEFT,
                LSM_FP_RIGHT,
                LSM_FP_TOP,
                LSM_FP_BOTTOM
            },
            {	// LSM_FP_FAR
                LSM_FP_LEFT,
                LSM_FP_RIGHT,
                LSM_FP_TOP,
                LSM_FP_BOTTOM
            },
        };

        for (uint32_t I = 4UL; I--;) {
            _fpRet[I] = fpTable[index][I];
        }
    }


    void GetCornersOfPlanes(LSM_FRUSTUM_PLANES _fpPlane0, LSM_FRUSTUM_PLANES _fpPlane1, LSM_FRUSTUM_POINTS _fpRet[2]) {
        static const LSM_FRUSTUM_POINTS fpTable[LSM_FP_TOTAL][LSM_FP_TOTAL][2] = {
            {	// LSM_FP_LEFT
                {	// LSM_FP_LEFT
                    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,		// Invalid combination.
                },
                {	// LSM_FP_RIGHT
                    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,		// Invalid combination.
                },
                {	// LSM_FP_TOP
                    LSM_FP_NEAR_TOP_LEFT, LSM_FP_FAR_TOP_LEFT,
                },
                {	// LSM_FP_BOTTOM
                    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,
                },
                {	// LSM_FP_NEAR
                    LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_NEAR_TOP_LEFT,
                },
                {	// LSM_FP_FAR
                    LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_BOTTOM_LEFT,
                },
            },
            {	// LSM_FP_RIGHT
                {	// LSM_FP_LEFT
                    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,	// Invalid combination.
                },
                {	// LSM_FP_RIGHT
                    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,	// Invalid combination.
                },
                {	// LSM_FP_TOP
                    LSM_FP_FAR_TOP_RIGHT, LSM_FP_NEAR_TOP_RIGHT,
                },
                {	// LSM_FP_BOTTOM
                    LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,
                },
                {	// LSM_FP_NEAR
                    LSM_FP_NEAR_TOP_RIGHT, LSM_FP_NEAR_BOTTOM_RIGHT,
                },
                {	// LSM_FP_FAR
                    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_TOP_RIGHT,
                },
            },

            // ==

            {	// LSM_FP_TOP
                {	// LSM_FP_LEFT
                    LSM_FP_FAR_TOP_LEFT, LSM_FP_NEAR_TOP_LEFT,
                },
                {	// LSM_FP_RIGHT
                    LSM_FP_NEAR_TOP_RIGHT, LSM_FP_FAR_TOP_RIGHT,
                },
                {	// LSM_FP_TOP
                    LSM_FP_NEAR_TOP_LEFT, LSM_FP_FAR_TOP_LEFT,		// Invalid combination.
                },
                {	// LSM_FP_BOTTOM
                    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,	// Invalid combination.
                },
                {	// LSM_FP_NEAR
                    LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_TOP_RIGHT,
                },
                {	// LSM_FP_FAR
                    LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,
                },
            },
            {	// LSM_FP_BOTTOM
                {	// LSM_FP_LEFT
                    LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,
                },
                {	// LSM_FP_RIGHT
                    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_NEAR_BOTTOM_RIGHT,
                },
                {	// LSM_FP_TOP
                    LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_LEFT,	// Invalid combination.
                },
                {	// LSM_FP_BOTTOM
                    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,	// Invalid combination.
                },
                {	// LSM_FP_NEAR
                    LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_NEAR_BOTTOM_LEFT,
                },
                {	// LSM_FP_FAR
                    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_BOTTOM_RIGHT,
                },
            },

            // ==

            {	// LSM_FP_NEAR
                {	// LSM_FP_LEFT
                    LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_BOTTOM_LEFT,
                },
                {	// LSM_FP_RIGHT
                    LSM_FP_NEAR_BOTTOM_RIGHT, LSM_FP_NEAR_TOP_RIGHT,
                },
                {	// LSM_FP_TOP
                    LSM_FP_NEAR_TOP_RIGHT, LSM_FP_NEAR_TOP_LEFT,
                },
                {	// LSM_FP_BOTTOM
                    LSM_FP_NEAR_BOTTOM_LEFT, LSM_FP_NEAR_BOTTOM_RIGHT,
                },
                {	// LSM_FP_NEAR
                    LSM_FP_NEAR_TOP_LEFT, LSM_FP_NEAR_TOP_RIGHT,		// Invalid combination.
                },
                {	// LSM_FP_FAR
                    LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,		// Invalid combination.
                },
            },
            {	// LSM_FP_FAR
                {	// LSM_FP_LEFT
                    LSM_FP_FAR_BOTTOM_LEFT, LSM_FP_FAR_TOP_LEFT,
                },
                {	// LSM_FP_RIGHT
                    LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_BOTTOM_RIGHT,
                },
                {	// LSM_FP_TOP
                    LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_TOP_RIGHT,
                },
                {	// LSM_FP_BOTTOM
                    LSM_FP_FAR_BOTTOM_RIGHT, LSM_FP_FAR_BOTTOM_LEFT,
                },
                {	// LSM_FP_NEAR
                    LSM_FP_FAR_TOP_LEFT, LSM_FP_FAR_TOP_RIGHT,		// Invalid combination.
                },
                {	// LSM_FP_FAR
                    LSM_FP_FAR_TOP_RIGHT, LSM_FP_FAR_TOP_LEFT,		// Invalid combination.
                },
            },
        };
        _fpRet[0] = fpTable[_fpPlane0][_fpPlane1][0];
        _fpRet[1] = fpTable[_fpPlane0][_fpPlane1][1];
    }


    template <uint32_t MAX_KDOP_PLANES>
    void MakeKDop(KDOP<MAX_KDOP_PLANES>& kdop, const std::array<Plane, 6>& frustumPlanes, const FrustumCorners& frustumCorners, const Vec3& lightDir) {
        kdop.Reset();
        // Add planes that are facing towards us.
        for (uint32_t index = 0; index < frustumPlanes.size(); index++) {
            const Plane& plane = frustumPlanes.at(index);
            float fDir = glm::dot(plane.mNormal, lightDir);
            if (fDir < 0.0f) {
                kdop.AddPlane(plane);
            }
        }

        // We have added the back sides of the planes.  Now find the edges.
        // For each plane.
        for (uint32_t index = 0; index < frustumPlanes.size(); index++) {
            const Plane& plane = frustumPlanes.at(index);
            // If this plane is facing away from us, move on.
            float fDir = glm::dot(plane.mNormal, lightDir);
            if (fDir > 0.0f) { continue; }
            // For each neighbor of this plane.
            LSM_FRUSTUM_PLANES fpNeighbors[4];
            GetNeighbors(index, fpNeighbors);
            for (uint32_t J = 4UL; J--;) {
                float fNeighborDir = glm::dot(frustumPlanes[fpNeighbors[J]].mNormal, lightDir);
                // If this plane is facing away from us, the edge between plane I and plane J
                //	marks the edge of a plane we need to add.
                if (fNeighborDir > 0.0f) {
                    LSM_FRUSTUM_POINTS fpPoints[2];
                    GetCornersOfPlanes(static_cast<LSM_FRUSTUM_PLANES>(index), fpNeighbors[J], fpPoints);
                    Plane pAddMe(Vec3(frustumCorners[fpPoints[0]]), Vec3(frustumCorners[fpPoints[1]]), Vec3(frustumCorners[fpPoints[0]]) + lightDir);
                    kdop.AddPlane(pAddMe);
                }
            }
        }
    }
    */








    void AddMesh(std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const Mat4& worldMatrix)
    {
		resultMeshes.emplace_back(mesh.mMeshID, worldMatrix, mesh.GetMaterial()->mDiffuseTexture, mesh.GetMaterial()->mNormalTexture, mesh.GetMaterial()->mSpecularFactor, mesh.GetTextureTilingFactor());
    }

    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& worldMatrix)
    {
        resultMeshes.emplace_back(mesh.mMeshID, worldMatrix);
    }

    template <typename T>
    void AddAllMeshes(std::vector<T>& resultMeshes, ModelNode& node, const Mat4& worldMatrix)
    {
		for (const Mesh& mesh : node.GetMeshes())
            AddMesh(resultMeshes, mesh, worldMatrix * node.mLocalTransform);

		for (ModelNode& node : node.GetChildNodes())
            AddAllMeshes(resultMeshes, node, worldMatrix);
    }

    template <typename T>
    void CullMeshesFrustrum(std::vector<T>& resultMeshes, ModelNode& node, const Mat4& wvpMatrix, const Mat4& worldMatrix)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.mLocalTransform;

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
                        AddMesh(resultMeshes, mesh, worldMatrix * node.mLocalTransform);
				}

                // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
				for (ModelNode& node : node.GetChildNodes())
                    CullMeshesFrustrum(resultMeshes, node, wvpMatrix, worldMatrix);

				break;
			}

            case AABBIntersection::Inside:
			{
                AddAllMeshes(resultMeshes, node, worldMatrix);
				break;
			}

            case AABBIntersection::Outside:
			default:
				break;
		}
    }

    void CullMeshesAABB(std::vector<RenderableMesh>& resultMeshes, ModelNode& node, const AABB& aabb, const Mat4& worldMatrix)
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
                        AddMesh(resultMeshes, mesh, worldMatrix * node.mLocalTransform);
                }

                // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
                for (ModelNode& node : node.GetChildNodes())
                    CullMeshesAABB(resultMeshes, node, aabb, worldMatrix);

                break;
            }

            case AABBIntersection::Inside:
        {
            AddAllMeshes(resultMeshes, node, worldMatrix);
            break;
        }

        case AABBIntersection::Outside:
        default:
            break;
        }
    }

	void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius)
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
                        AddMesh(resultMeshes, mesh, worldMatrix * node.mLocalTransform);
				}

				// each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
				for (ModelNode& node : node.GetChildNodes())
					CullMeshesSphere(resultMeshes, node, worldMatrix, sphereCentre, sphereRadius);

				break;
			}

            case AABBIntersection::Inside:
			{
				AddAllMeshes(resultMeshes, node, worldMatrix);
				break;
			}

            case AABBIntersection::Outside:
			default:
				break;
        }
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

        /*FrustumPlanes frustumPlanes = GetFrustumPlanes(mRenderQueue.mCamera.mCameraViewProjectionMatrix);

        // test kdop
        const ActorPtr& actor = mActors.front();
        const Mat4 worldMatrix = actor->mSceneNode->GetWorldMatrix() * actor->mModel->GetRootNode().mLocalTransform;
        const AABB& localAABB = actor->mModel->GetRootNode().mLocalAABB;
        const AABB aabb = localAABB * worldMatrix;

       // AABBIntersection intersection = Intersection<11>(aabb, kdop);

        for (Plane& plane : frustumPlanes)
        {
            if (Intersection(plane, aabb) == PlaneIntersection::Back)
                assert(1 == 0);
        }*/

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
        FrustumCorners cameraFrustrum = GetFrustumCorners(mRenderQueue.mCamera.mCameraViewProjectionMatrix);
        for (const DirectionalLightPtr& dirLight : mDirectionalLights)
        {
            mRenderQueue.mDirectionalLights.emplace_back(dirLight->mLightColor, dirLight->mLightDirection);
            RenderableDirLight& dirLight = mRenderQueue.mDirectionalLights.back();
            
            /*const AABB cameraAABB(cameraFrustrum);
            for (const ActorPtr& actor : mActors)
            {
                if (!actor->mSceneNode)
                    continue;

                const Mat4& worldMatrix = actor->mSceneNode->GetWorldMatrix();
                // TODO - not working?
                CullMeshesAABB(dirLight.mMeshes, actor->mModel->GetRootNode(), cameraAABB, worldMatrix);
            }*/
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