#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/IDMapTree.hpp"
#include "include/Core/Utils/Time.h"
#include "include/Scene/AnimatedActor.h"
#include "include/Scene/Camera.h"
#include "include/Scene/DirectionalLight.h"
#include "include/Scene/PointLight.h"
#include "include/Scene/SceneNode.h"
#include "include/Scene/StaticActor.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/RenderQueue.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class Scene
    {
    public:
        Scene(const std::string& sceneName, DX11Renderer& renderer, const ResourceManifest& resourceManifest);
        ~Scene();

        void Tick(const Milliseconds elapsedTime, const float windowWidth, const float windowHeight);
        const RenderQueue& GetRenderQueue();

        SceneNode& GetRootNode();
        const SceneNodeID GetRootNodeID() const;

        SceneNodeID CreateSceneNode(const std::string& sceneNodeName, const SceneNodeID parent);
        void DeleteSceneNode(SceneNodeID& sceneNodeID);
        SceneNode& GetSceneNode(const SceneNodeID sceneNodeID);

        StaticActorID CreateStaticActor(const std::string& actorName, const ModelID modelID, const SceneNodeID sceneNodeID);
        void DeleteStaticActor(StaticActorID& actorID);
        StaticActor& GetStaticActor(const StaticActorID actorID);

        AnimatedActorID CreateAnimatedActor(const std::string& actorName, const ModelID modelID, const SceneNodeID sceneNodeID);
        void DeleteAnimatedActor(AnimatedActorID& actorID);
        AnimatedActor& GetAnimatedActor(const AnimatedActorID actorID);

        PointLightID CreatePointLight(const std::string& lightName, const SceneNodeID sceneNodeID);
        void DeletePointLight(PointLightID& pointLightID);
		PointLight& GetPointLight(const PointLightID pointLightID);
        
        DirectionalLightID CreateDirectionalLight(const std::string& lightName);
        DirectionalLightID CreateDirectionalLight(const std::string& lightName, const uint32_t numShadowmapCascades);
        void DeleteDirectionalLight(DirectionalLightID& dirLightID);
        DirectionalLight& GetDirectionalLight(const DirectionalLightID dirLightID);

		void SetAmbientLight(const Vec4& ambientLight);
        const Vec4& GetAmbientLight() const;

        void SetSkybox(const SkyboxID skyboxID);
        SkyboxID GetSkybox() const;

        Camera& GetSceneCamera();
        const Camera& GetSceneCamera() const;


		const std::string mName;


    private:
        void UpdateDirtyObjects();
        void MarkAsDirty(SceneNode* sceneNode);
        void UpdateDirLightSplitRanges(const float windowWidth, const float windowHeight);


        DX11Renderer& mRenderer;
        const ResourceManifest& mResourceManifest;

		Camera mSceneCamera;
        Vec4 mAmbientLight;
        SkyboxID mSkyboxID;

		IDMap<PointLight> mPointLights;
        IDMap<DirectionalLight> mDirectionalLights;
        IDMap<StaticActor> mStaticActors;
        IDMap<AnimatedActor> mAnimatedActors;
        IDMapTree<SceneNode> mSceneNodeTree;

        bool mHasDirtySceneNodes;

        const SceneNodeID mRootNodeID;
        RenderQueue mRenderQueue;

        float mPrevMinCameraDepth;
        float mPrevMaxCameraDepth;
    };
}