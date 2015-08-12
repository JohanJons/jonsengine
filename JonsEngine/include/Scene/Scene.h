#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/DataPool.hpp"
#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Scene/PointLight.h"
#include "include/Scene/DirectionalLight.h"
#include "include/Scene/Actor.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/RenderQueue.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class IMemoryAllocator;

    class Scene
    {
    public:
        Scene(const std::string& sceneName, const ResourceManifest& resourceManifest);
        ~Scene();

        const RenderQueue& GetRenderQueue(const Mat4& cameraProjectionMatrix, const float fov, const float aspectRatio, const float minDepth, const float maxDepth);

        ActorID CreateActor(const std::string& actorName, const ModelID modelID, const SceneNodePtr node);
        void DeleteActor(ActorID& actorID);
        Actor& GetActor(const ActorID actorID);
        
		PointLightID CreatePointLight(const std::string& lightName, SceneNodePtr node);
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
        SceneNode& GetRootNode();


		const std::string mName;


    private:
        void UpdateDirtyObjects();


        IMemoryAllocator& mMemoryAllocator;
        const ResourceManifest& mResourceManifest;
        DataPool<Mat4> mTransformCache;

        std::vector<SceneNode*> mDirtySceneNodes;

		Camera mSceneCamera;
        SceneNode mRootNode;
        Vec4 mAmbientLight;
        SkyboxID mSkyboxID;

		IDMap<PointLight> mPointLights;
        IDMap<DirectionalLight> mDirectionalLights;
        IDMap<Actor> mActors;

        RenderQueue mRenderQueue;
    };

    typedef std::unique_ptr<Scene, std::function<void(Scene*)>> ScenePtr;
}