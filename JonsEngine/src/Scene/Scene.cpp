#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Math/Intersection.h"

#include <algorithm>
#include <functional>

namespace JonsEngine
{
    //
    // TODO: all this code related to culling/building renderqueue should be moved elsewhere
    //
    void AddMesh(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix, const float tilingFactor, const MaterialID actorMaterial);
    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix);
    void AddAllMeshes(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const float tilingFactor, const MaterialID actorMaterial);
    void AddAllMeshes(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix);
    void CullMeshesFrustrum(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Mat4& wvpMatrix, const float tilingFactor, const MaterialID actorMaterial);
    //void CullMeshesAABB(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const AABB& aabb, const Mat4& worldMatrix);
    void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius);


    Scene::Scene(const std::string& sceneName, const ResourceManifest& resourceManifest) :
        mName(sceneName), mResourceManifest(resourceManifest), mAmbientLight(0.2f), mSkyboxID(IDMap<Skybox>::INVALID_ITEM_ID), mRootNodeID(CreateSceneNode("Root", INVALID_SCENE_NODE_ID))
    {
    }

    Scene::~Scene()
    {
    }


    SceneNode& Scene::GetRootNode()
    {
        return mSceneNodes.GetItem(mRootNodeID);
    }

    const SceneNodeID Scene::GetRootNodeID() const
    {
        return mRootNodeID;
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

		for (const Actor& actor : mActors)
		{
            if (actor.mSceneNodeID == INVALID_SCENE_NODE_ID || actor.mModelID == INVALID_MODEL_ID)
				continue;

            const SceneNode& sceneNode = mSceneNodes.GetItem(actor.mSceneNodeID);

            const Mat4& worldMatrix = sceneNode.GetWorldTransform();
            const Mat4 wvpMatrix = mRenderQueue.mCamera.mCameraViewProjectionMatrix * worldMatrix;

            const Model& model = mResourceManifest.GetModel(actor.mModelID);

            CullMeshesFrustrum(mResourceManifest, mRenderQueue.mCamera.mModels, model.GetRootNode(), worldMatrix, wvpMatrix, actor.mMaterialTilingFactor, actor.mMaterialID);
		}

        // point lights
        for (const PointLight& pointLight : mPointLights)
        {
            if (pointLight.mSceneNodeID == INVALID_SCENE_NODE_ID)
                continue;

            const SceneNode& sceneNode = mSceneNodes.GetItem(pointLight.mSceneNodeID);

            const Vec3 lightPosition = sceneNode.Position();

            mRenderQueue.mPointLights.emplace_back(pointLight.mLightColor, lightPosition, pointLight.mLightIntensity, pointLight.mLightRadius);
            RenderablePointLight& renderablePointLight = mRenderQueue.mPointLights.back();

            //  cull meshes for each face
            for (const Actor& actor : mActors)
            {
                if (actor.mSceneNodeID == INVALID_SCENE_NODE_ID || actor.mModelID == INVALID_MODEL_ID)
                    continue;

                const SceneNode& sceneNode = mSceneNodes.GetItem(actor.mSceneNodeID);

                const Mat4& actorWorldMatrix = sceneNode.GetWorldTransform();
                const Model& model = mResourceManifest.GetModel(actor.mModelID);
                CullMeshesSphere(renderablePointLight.mMeshes, model.GetRootNode(), actorWorldMatrix, lightPosition, pointLight.mLightRadius);
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
                for (const Actor& actor : mActors)
                {
                    if (actor.mSceneNodeID == INVALID_SCENE_NODE_ID || actor.mModelID == INVALID_MODEL_ID)
                        continue;

                    const SceneNode& sceneNode = mSceneNodes.GetItem(actor.mSceneNodeID);
                    const Model& model = mResourceManifest.GetModel(actor.mModelID);

                    const Mat4& worldMatrix = sceneNode.GetWorldTransform();
                    const Mat4 localWorldMatrix = worldMatrix * model.GetRootNode().mLocalTransform;
                    const AABB worldAABB = localWorldMatrix * model.GetRootNode().mLocalAABB;

                    const auto aabbIntersection = Intersection(worldAABB, kdopIterator);
                    if (aabbIntersection == AABBIntersection::Inside || aabbIntersection == AABBIntersection::Partial)
                        AddAllMeshes(renderableDirLight.mMeshes, model.GetRootNode(), worldMatrix);
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


    SceneNodeID Scene::CreateSceneNode(const std::string& sceneNodeName, const SceneNodeID parent)
    {
        SceneNodeID ret = mSceneNodes.AddItem(sceneNodeName, parent, std::bind(&Scene::MarkAsDirty, this, std::placeholders::_1));
       // mSceneNodeIters.emplace_back(ret, parent, nullptr, nullptr);

        //SceneNodeIterator& lastChild = FindLastChild(parent);
        //lastChild.mNextSibling = ret;

        return ret;
    }

    void Scene::DeleteSceneNode(SceneNodeID& sceneNodeID)
    {
        assert(sceneNodeID != INVALID_SCENE_NODE_ID);

        mSceneNodes.MarkAsFree(sceneNodeID);
        sceneNodeID = INVALID_SCENE_NODE_ID;
    }

    SceneNode& Scene::GetSceneNode(const SceneNodeID sceneNodeID)
    {
        return mSceneNodes.GetItem(sceneNodeID);
    }


    ActorID Scene::CreateActor(const std::string& actorName, const ModelID modelID, const SceneNodeID node)
    {
        return mActors.AddItem(actorName, modelID, node);
    }

    void Scene::DeleteActor(ActorID& actorID)
    {
        assert(actorID != INVALID_ACTOR_ID);

        mActors.MarkAsFree(actorID);
        actorID = INVALID_ACTOR_ID;
    }

    Actor& Scene::GetActor(const ActorID actorID)
    {
        return mActors.GetItem(actorID);
    }


	PointLightID Scene::CreatePointLight(const std::string& lightName, const SceneNodeID node)
    {
        return mPointLights.AddItem(lightName, node);
    }
    
	void Scene::DeletePointLight(PointLightID& pointLightID)
    {
        assert(pointLightID != INVALID_POINT_LIGHT_ID);

		mPointLights.MarkAsFree(pointLightID);
        pointLightID = INVALID_POINT_LIGHT_ID;
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

    void Scene::DeleteDirectionalLight(DirectionalLightID& dirLightID)
    {
        assert(dirLightID != INVALID_DIRECTIONAL_LIGHT_ID);

        mDirectionalLights.MarkAsFree(dirLightID);
        dirLightID = INVALID_DIRECTIONAL_LIGHT_ID;
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


    void Scene::UpdateDirtyObjects()
    {
        if (mDirtySceneNode)
        {
            //for (auto iter = mSceneNodes.begin() + 1; iter != mSceneNodes.end(); ++iter)
            //{
                //iter
                //const Mat4& parentTransform = mSceneNodes.GetItem(iter->)

                //node.UpdateWorldMatrix(*parentTransform);
                //parentTransform 
            //}
        }

        mDirtySceneNode = false;
    }

    void Scene::MarkAsDirty(SceneNode* sceneNode)
    {
    }

    /*SceneNodeIterator& Scene::FindSceneNodeIterator(const SceneNodeID sceneNodeID)
    {
        assert(sceneNodeID != INVALID_SCENE_NODE_ID);

        auto iter = std::find_if(mSceneNodeIters.begin(), mSceneNodeIters.end(), [sceneNodeID](const SceneNodeIterator iter) { return sceneNodeID == iter.mSceneNodeID; });
        assert(iter != mSceneNodeIters.end());

        return *iter;
    }

    SceneNodeIterator* Scene::FindLastChild(const SceneNodeID parentID)
    {
        //auto iter = std::find_if(mSceneNodeIters.begin(), mSceneNodeIters.end(), [parentID](const SceneNodeIterator iter) { return parentID == iter. && iter.mNextSibling == nullptr; });

        //return iter._Ptr;
        return nullptr;
    }*/


    void AddMesh(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        auto diffuseTexture = INVALID_DX11_MATERIAL_ID;
        auto normalTexture = INVALID_DX11_MATERIAL_ID;
        // TODO
        auto specularFactor = 0.02f;

        const bool actorHasMaterial = actorMaterial != INVALID_MATERIAL_ID;
        const MaterialID materialID = actorHasMaterial ? actorMaterial : mesh.mDefaultMaterialID;
        if (materialID != INVALID_MATERIAL_ID)
        {
            const Material& material = resourceManifest.GetMaterial(materialID);
            diffuseTexture = material.mDiffuseTexture;
            normalTexture = material.mNormalTexture;
            specularFactor = material.mSpecularFactor;
        }

        resultMeshes.emplace_back(mesh.mMeshID, localWorldMatrix, diffuseTexture, normalTexture, specularFactor, tilingFactor);
    }

    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix)
    {
        resultMeshes.emplace_back(mesh.mMeshID, localWorldMatrix);
    }

    void AddAllMeshes(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        for (const Mesh& mesh : node.mMeshes)
            AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * node.mLocalTransform, tilingFactor, actorMaterial);

        for (const ModelNode& child : node.mAllChildNodes)
        {
            for (const Mesh& mesh : child.mMeshes)
                AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * child.mLocalTransform, tilingFactor, actorMaterial);
        }
    }

    void AddAllMeshes(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix)
    {
        for (const Mesh& mesh : node.mMeshes)
            AddMesh(resultMeshes, mesh, worldMatrix * node.mLocalTransform);

        for (const ModelNode& child : node.mAllChildNodes)
        {
            for (const Mesh& mesh : child.mMeshes)
                AddMesh(resultMeshes, mesh, worldMatrix * child.mLocalTransform);
        }
    }

    void CullMeshesFrustrum(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Mat4& wvpMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.mLocalTransform;
        const Mat4 localWorldMatrix = worldMatrix * node.mLocalTransform;
        const AABB nodeWorldAABB = localWorldMatrix * node.mLocalAABB;

        AABBIntersection nodeAABBIntersection = Intersection(nodeWorldAABB, localWVPMatrix);
        switch (nodeAABBIntersection)
        {
            // if partially inside, recursively test all meshes and child nodes
        case AABBIntersection::Partial:
        {
            AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

            for (const Mesh& mesh : node.mMeshes)
            {
                const AABB meshWorldAABB = localWorldMatrix * mesh.mLocalAABB;

                meshAABBIntersection = Intersection(meshWorldAABB, localWVPMatrix);
                if (meshAABBIntersection == AABBIntersection::Outside)
                    continue;

                if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                    AddMesh(resourceManifest, resultMeshes, mesh, localWorldMatrix, tilingFactor, actorMaterial);
            }

            // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
            for (const ModelNode& child : node.mImmediateChildNodes)
                CullMeshesFrustrum(resourceManifest, resultMeshes, child, worldMatrix, wvpMatrix, tilingFactor, actorMaterial);

            break;
        }

        case AABBIntersection::Inside:
        {
            AddAllMeshes(resourceManifest, resultMeshes, node, worldMatrix, tilingFactor, actorMaterial);
            break;
        }

        case AABBIntersection::Outside:
        default:
            break;
        }
    }

    /*void CullMeshesAABB(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const AABB& aabb, const Mat4& worldMatrix)
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
                    AddMesh(resultMeshes, mesh, worldMatrix * node.mLocalTransform);
            }

            // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
            for (const ModelNode& child : node.mImmediateChildNodes)
                CullMeshesAABB(resultMeshes, child, aabb, worldMatrix);

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
    }*/

    void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius)
    {
        const Mat4 localWorldMatrix = worldMatrix * node.mLocalTransform;
        const AABB nodeWorldAABB = localWorldMatrix * node.mLocalAABB;

        // test node frustrum
        AABBIntersection nodeAABBIntersection = Intersection(nodeWorldAABB, sphereCentre, sphereRadius);
        switch (nodeAABBIntersection)
        {
        case AABBIntersection::Partial:
        {
            AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

            for (const Mesh& mesh : node.mMeshes)
            {
                const AABB worldMeshAABB = localWorldMatrix * node.mLocalAABB;

                meshAABBIntersection = Intersection(worldMeshAABB, sphereCentre, sphereRadius);
                if (meshAABBIntersection == AABBIntersection::Outside)
                    continue;

                if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                    AddMesh(resultMeshes, mesh, localWorldMatrix);
            }

            // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
            for (const ModelNode& child : node.mImmediateChildNodes)
                CullMeshesSphere(resultMeshes, child, worldMatrix, sphereCentre, sphereRadius);

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
}