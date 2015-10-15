#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
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
    void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius);


    Scene::Scene(const std::string& sceneName, DX11Renderer& renderer, const ResourceManifest& resourceManifest) :
        mName(sceneName),
        mRenderer(renderer),
        mResourceManifest(resourceManifest),
        mAmbientLight(0.2f),
        mSkyboxID(INVALID_SKYBOX_ID),
        mSceneNodeTree("Root", INVALID_SCENE_NODE_ID, std::bind(&Scene::MarkAsDirty, this, std::placeholders::_1)),
        mRootNodeID(mSceneNodeTree.GetRootNodeID())
    {
    }

    Scene::~Scene()
    {
    }


    void Scene::Tick(const Milliseconds elapsedTime, const float windowWidth, const float windowHeight)
    {
        const Camera& sceneCamera = GetSceneCamera();
        const float cameraFov = sceneCamera.GetFOV();
        const float windowAspectRatio = windowWidth / static_cast<float>(windowHeight);

        UpdateRenderQueueCamera(cameraFov, windowAspectRatio);
        UpdateDirtyObjects();
        UpdateDirLightSplitRanges(cameraFov, windowAspectRatio);
        UpdateAnimatedActors(elapsedTime);
    }

    const RenderQueue& Scene::GetRenderQueue()
    {
        mRenderQueue.Clear();

        for (const StaticActor& actor : mStaticActors)
        {
            const SceneNodeID sceneNodeID = actor.GetSceneNode();
            const ModelID modelID = actor.GetModel();

            if (sceneNodeID == INVALID_SCENE_NODE_ID || modelID == INVALID_MODEL_ID)
                continue;

            const SceneNode& sceneNode = mSceneNodeTree.GetNode(sceneNodeID);

            const Mat4& worldMatrix = sceneNode.GetWorldTransform();
            const Mat4 wvpMatrix = mRenderQueue.mCamera.mCameraViewProjectionMatrix *worldMatrix;

            const Model& model = mResourceManifest.GetModel(modelID);

            CullMeshesFrustrum(mResourceManifest, mRenderQueue.mCamera.mModels, model.GetRootNode(), worldMatrix, wvpMatrix, actor.GetMaterialTilingFactor(), actor.GetMaterial());
        }

        // point lights
        for (const PointLight& pointLight : mPointLights)
        {
            if (pointLight.GetSceneNode() == INVALID_SCENE_NODE_ID)
                continue;

            const SceneNode& sceneNode = mSceneNodeTree.GetNode(pointLight.GetSceneNode());

            const Vec3 lightPosition = sceneNode.Position();

            mRenderQueue.mPointLights.emplace_back(pointLight.GetLightColor(), lightPosition, pointLight.GetIntensity(), pointLight.GetRadius());
            RenderablePointLight& renderablePointLight = mRenderQueue.mPointLights.back();

            //  cull meshes for each face
            for (const StaticActor& actor : mStaticActors)
            {
                const SceneNodeID sceneNodeID = actor.GetSceneNode();
                const ModelID modelID = actor.GetModel();

                if (sceneNodeID == INVALID_SCENE_NODE_ID || modelID == INVALID_MODEL_ID)
                    continue;

                const SceneNode& sceneNode = mSceneNodeTree.GetNode(sceneNodeID);

                const Mat4& actorWorldMatrix = sceneNode.GetWorldTransform();
                const Model& model = mResourceManifest.GetModel(modelID);
                CullMeshesSphere(renderablePointLight.mMeshes, model.GetRootNode(), actorWorldMatrix, lightPosition, pointLight.GetRadius());
            }
        }

        // dir lights
        for (DirectionalLight& dirLight : mDirectionalLights)
        {
            mRenderQueue.mDirectionalLights.emplace_back(dirLight.GetLightColor(), glm::normalize(dirLight.GetLightDirection()), dirLight.GetNumCascades());
            RenderableDirLight& renderableDirLight = mRenderQueue.mDirectionalLights.back();

            for (uint32_t cascadeIndex = 0; cascadeIndex < dirLight.GetNumCascades(); ++cascadeIndex)
            {
                float nearZ = 0.0f, farZ = 0.0f;
                dirLight.GetSplitDistance(cascadeIndex, nearZ, farZ);

                auto kdopIterator = dirLight.GetBoundingVolume(cascadeIndex);
                for (const StaticActor& actor : mStaticActors)
                {
                    if (actor.GetSceneNode() == INVALID_SCENE_NODE_ID || actor.GetModel() == INVALID_MODEL_ID)
                        continue;

                    const SceneNode& sceneNode = mSceneNodeTree.GetNode(actor.GetSceneNode());
                    const Model& model = mResourceManifest.GetModel(actor.GetModel());

                    const Mat4& worldMatrix = sceneNode.GetWorldTransform();
                    const Mat4 localWorldMatrix = worldMatrix;// *model.GetRootNode().mLocalTransform;
                    const AABB worldAABB = localWorldMatrix * model.GetRootNode().GetLocalAABB();

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
        mRenderQueue.mSkyboxTextureID = skybox.GetSkyboxTexture();

        return mRenderQueue;
    }


    SceneNode& Scene::GetRootNode()
    {
        return mSceneNodeTree.GetNode(mRootNodeID);
    }

    const SceneNodeID Scene::GetRootNodeID() const
    {
        return mRootNodeID;
    }

    
    SceneNodeID Scene::CreateSceneNode(const std::string& sceneNodeName, const SceneNodeID parent)
    {
        return mSceneNodeTree.AddNode(parent, sceneNodeName, parent, std::bind(&Scene::MarkAsDirty, this, std::placeholders::_1));
    }

    void Scene::DeleteSceneNode(SceneNodeID& sceneNodeID)
    {
        assert(sceneNodeID != INVALID_SCENE_NODE_ID);

        mSceneNodeTree.FreeNode(sceneNodeID);
        sceneNodeID = INVALID_SCENE_NODE_ID;
    }

    SceneNode& Scene::GetSceneNode(const SceneNodeID sceneNodeID)
    {
        return mSceneNodeTree.GetNode(sceneNodeID);
    }


    StaticActorID Scene::CreateStaticActor(const std::string& actorName, const ModelID modelID, const SceneNodeID sceneNodeID)
    {
        return mStaticActors.Insert(actorName, modelID, sceneNodeID);
    }

    void Scene::DeleteStaticActor(StaticActorID& actorID)
    {
        assert(actorID != INVALID_STATIC_ACTOR_ID);

        mStaticActors.Erase(actorID);
        actorID = INVALID_STATIC_ACTOR_ID;
    }

    StaticActor& Scene::GetStaticActor(const StaticActorID actorID)
    {
        return mStaticActors.GetItem(actorID);
    }


    AnimatedActorID Scene::CreateAnimatedActor(const std::string& actorName, const ModelID modelID, const SceneNodeID sceneNodeID)
    {
        return mAnimatedActors.Insert(actorName, modelID, sceneNodeID);
    }

    void Scene::DeleteAnimatedActor(AnimatedActorID& actorID)
    {
        assert(actorID != INVALID_ANIMATED_ACTOR_ID);

        mAnimatedActors.Erase(actorID);
        actorID = INVALID_ANIMATED_ACTOR_ID;
    }

    AnimatedActor& Scene::GetAnimatedActor(const AnimatedActorID actorID)
    {
        return mAnimatedActors.GetItem(actorID);
    }


	PointLightID Scene::CreatePointLight(const std::string& lightName, const SceneNodeID node)
    {
        return mPointLights.Insert(lightName, node);
    }
    
	void Scene::DeletePointLight(PointLightID& pointLightID)
    {
        assert(pointLightID != INVALID_POINT_LIGHT_ID);

		mPointLights.Erase(pointLightID);
        pointLightID = INVALID_POINT_LIGHT_ID;
    }
    
	PointLight& Scene::GetPointLight(const PointLightID pointLightID)
    {
		return mPointLights.GetItem(pointLightID);
    }
    

    DirectionalLightID Scene::CreateDirectionalLight(const std::string& lightName)
    {
        const uint32_t defaultNumCascades = 4;

        return mDirectionalLights.Insert(lightName, defaultNumCascades);
    }

    DirectionalLightID Scene::CreateDirectionalLight(const std::string& lightName, const uint32_t numShadowmapCascades)
    {
        return mDirectionalLights.Insert(lightName, numShadowmapCascades);
    }

    void Scene::DeleteDirectionalLight(DirectionalLightID& dirLightID)
    {
        assert(dirLightID != INVALID_DIRECTIONAL_LIGHT_ID);

        mDirectionalLights.Erase(dirLightID);
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


    void Scene::MarkAsDirty(SceneNode* sceneNode)
    {
        mHasDirtySceneNodes = true;
    }

    void Scene::UpdateRenderQueueCamera(const float cameraFov, const float windowAspectRatio)
    {
        const Camera& sceneCamera = GetSceneCamera();

        mRenderQueue.mCamera.mFOV = cameraFov;
        mRenderQueue.mCamera.mCameraPosition = sceneCamera.Position();
        mRenderQueue.mCamera.mCameraViewMatrix = sceneCamera.GetCameraTransform();
        mRenderQueue.mCamera.mCameraProjectionMatrix = PerspectiveMatrixFov(cameraFov, windowAspectRatio, mRenderer.GetZNear(), mRenderer.GetZFar());
        mRenderQueue.mCamera.mCameraViewProjectionMatrix = mRenderQueue.mCamera.mCameraProjectionMatrix * mRenderQueue.mCamera.mCameraViewMatrix;
    }

    void Scene::UpdateDirtyObjects()
    {
        if (!mHasDirtySceneNodes)
            return;

        // special case: root has no parent
        SceneNode& rootNode = mSceneNodeTree.GetNode(mRootNodeID);
        rootNode.UpdateWorldMatrix();

        // skips root
        for (auto nodeIter = mSceneNodeTree.begin() + 1; nodeIter != mSceneNodeTree.end(); ++nodeIter)
        {
            SceneNode& node = *nodeIter;
            SceneNode& parent = mSceneNodeTree.GetNode(node.GetParentID());
            const Mat4& parentWorldTransform = parent.GetWorldTransform();

            node.UpdateWorldMatrix(parentWorldTransform);
        }

        mHasDirtySceneNodes = false;
    }

    void Scene::UpdateDirLightSplitRanges(const float cameraFov, const float windowAspectRatio)
    {
        float minCameraDepth = 0.01f;
        float maxCameraDepth = 1.0f;
    
        mRenderer.ReduceDepth(mRenderQueue.mCamera.mCameraProjectionMatrix, minCameraDepth, maxCameraDepth);
        for (DirectionalLight& dirLight : mDirectionalLights)
            dirLight.UpdateCascadesBoundingVolume(mRenderQueue.mCamera.mCameraViewMatrix, cameraFov, windowAspectRatio, minCameraDepth, maxCameraDepth);
    }
    
    void Scene::UpdateAnimatedActors(const Milliseconds elapsedTime)
    {
        for (AnimatedActor& actor : mAnimatedActors)
        {
            if (!actor.IsPlaying())
                continue;
            
            actor.UpdateTimestamp(elapsedTime);
        }
    }


    void AddMesh(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        auto diffuseTexture = INVALID_DX11_MATERIAL_ID;
        auto normalTexture = INVALID_DX11_MATERIAL_ID;
        // TODO
        auto specularFactor = 0.02f;

        const bool actorHasMaterial = actorMaterial != INVALID_MATERIAL_ID;
        const MaterialID materialID = actorHasMaterial ? actorMaterial : mesh.GetDefaultMaterial();
        if (materialID != INVALID_MATERIAL_ID)
        {
            const Material& material = resourceManifest.GetMaterial(materialID);
            diffuseTexture = material.GetDiffuseTexture();
            normalTexture = material.GetNormalTexture();
            specularFactor = material.GetSpecularFactor();
        }

        resultMeshes.emplace_back(mesh.GetMesh(), localWorldMatrix, diffuseTexture, normalTexture, specularFactor, tilingFactor);
    }

    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix)
    {
        resultMeshes.emplace_back(mesh.GetMesh(), localWorldMatrix);
    }

    void AddAllMeshes(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        for (const Mesh& mesh : node.GetMeshes())
            AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * node.GetLocalTransform(), tilingFactor, actorMaterial);

        for (const ModelNode& child : node.GetAllChildren())
        {
            for (const Mesh& mesh : child.GetMeshes())
                AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * child.GetLocalTransform(), tilingFactor, actorMaterial);
        }
    }

    void AddAllMeshes(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix)
    {
        for (const Mesh& mesh : node.GetMeshes())
            AddMesh(resultMeshes, mesh, worldMatrix * node.GetLocalTransform());

        for (const ModelNode& child : node.GetAllChildren())
        {
            for (const Mesh& mesh : child.GetMeshes())
                AddMesh(resultMeshes, mesh, worldMatrix * child.GetLocalTransform());
        }
    }

    void CullMeshesFrustrum(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Mat4& wvpMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.GetLocalTransform();// *node.GetLocalTransform();// *node.mLocalTransform;
        const Mat4 localWorldMatrix = worldMatrix * node.GetLocalTransform();// *node.mLocalTransform;
        const AABB nodeWorldAABB = /*localWorldMatrix */ node.GetLocalAABB();

        AABBIntersection nodeAABBIntersection = Intersection(nodeWorldAABB, localWVPMatrix);
        switch (nodeAABBIntersection)
        {
            // if partially inside, recursively test all meshes and child nodes
        case AABBIntersection::Partial:
        {
            AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

            for (const Mesh& mesh : node.GetMeshes())
            {
                const AABB meshWorldAABB = /*localWorldMatrix */ mesh.GetLocalAABB();

                meshAABBIntersection = Intersection(meshWorldAABB, localWVPMatrix);
                if (meshAABBIntersection == AABBIntersection::Outside)
                    continue;

                if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                    AddMesh(resourceManifest, resultMeshes, mesh, /*worldMatrix */ localWorldMatrix, tilingFactor, actorMaterial);
            }

            // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
            for (const ModelNode& child : node.GetImmediateChildren())
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

    void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius)
    {
        const Mat4 localWorldMatrix = worldMatrix * node.GetLocalTransform();
        const AABB nodeWorldAABB = localWorldMatrix * node.GetLocalAABB();

        // test node frustrum
        AABBIntersection nodeAABBIntersection = Intersection(nodeWorldAABB, sphereCentre, sphereRadius);
        switch (nodeAABBIntersection)
        {
        case AABBIntersection::Partial:
        {
            AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

            for (const Mesh& mesh : node.GetMeshes())
            {
                const AABB worldMeshAABB = localWorldMatrix * node.GetLocalAABB();

                meshAABBIntersection = Intersection(worldMeshAABB, sphereCentre, sphereRadius);
                if (meshAABBIntersection == AABBIntersection::Outside)
                    continue;

                if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                    AddMesh(resultMeshes, mesh, localWorldMatrix);
            }

            // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
            for (const ModelNode& child : node.GetImmediateChildren())
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