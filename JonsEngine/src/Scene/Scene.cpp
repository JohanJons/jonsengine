#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"

#include <algorithm>
#include <functional>

namespace JonsEngine
{
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


    void Scene::Tick(const Milliseconds elapsedTime, const float windowAspectRatio)
    {
        const float cameraFov = GetSceneCamera().GetFOV();

        UpdateRenderQueueCamera(cameraFov, windowAspectRatio);
        UpdateDirtyObjects();
        UpdateDirLightSplitRanges(cameraFov, windowAspectRatio);
        UpdateAnimatedActors(elapsedTime);
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

    const SceneNode& Scene::GetSceneNode(const SceneNodeID sceneNodeID) const
    {
        return mSceneNodeTree.GetNode(sceneNodeID);
    }

    Scene::SceneNodeIterator Scene::GetSceneNodes() const
    {
        return SceneNodeIterator(mSceneNodeTree.cbegin(), mSceneNodeTree.cend());
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

    const StaticActor& Scene::GetStaticActor(const StaticActorID actorID) const
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

    const AnimatedActor& Scene::GetAnimatedActor(const AnimatedActorID actorID) const
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

    const PointLight& Scene::GetPointLight(const PointLightID pointLightID) const
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

    const DirectionalLight& Scene::GetDirectionalLight(const DirectionalLightID dirLightID) const
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
        const Mat4 viewMatrix = GetSceneCamera().GetCameraTransform();
        float minCameraDepth = 0.01f;
        float maxCameraDepth = 1.0f;
    
        mRenderer.ReduceDepth(minCameraDepth, maxCameraDepth);
        for (DirectionalLight& dirLight : mDirectionalLights)
            dirLight.UpdateCascadesBoundingVolume(viewMatrix, cameraFov, windowAspectRatio, minCameraDepth, maxCameraDepth);
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
}