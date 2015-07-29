#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/DataPool.hpp"
#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Scene/PointLight.h"
#include "include/Scene/DirectionalLight.h"
#include "include/Scene/Model.h"
#include "include/Scene/Actor.h"
#include "include/Scene/Skybox.h"
#include "include/Renderer/RenderQueue.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class IMemoryAllocator;

    typedef IDMap<PointLight>::ItemID PointLightID;
    typedef IDMap<PointLight>::ItemID DirectionalLightID;

    class Scene
    {
    public:
        Scene(const std::string& sceneName, const IDMap<Mat4>& modelTransformCache);
        ~Scene();

        bool operator==(const Scene& s1);
        bool operator==(const std::string& sceneName);

        const RenderQueue& GetRenderQueue(const Mat4& cameraProjectionMatrix, const float fov, const float aspectRatio, const float minDepth, const float maxDepth);

        Actor* CreateActor(const std::string& actorName, const ModelPtr model, const SceneNodePtr node);
        void DeleteActor(Actor* actor);
        Actor* GetActor(const std::string& actorName);
		const std::vector<ActorPtr>& GetActors() const;
        
		PointLightID CreatePointLight(const std::string& lightName, SceneNodePtr node);
        void DeletePointLight(const PointLightID pointLightID);
		PointLight& GetPointLight(const PointLightID pointLightID);
        
        DirectionalLightID CreateDirectionalLight(const std::string& lightName);
        DirectionalLightID CreateDirectionalLight(const std::string& lightName, const uint32_t numShadowmapCascades);
        void DeleteDirectionalLight(const DirectionalLightID dirLight);
        DirectionalLight& GetDirectionalLight(const DirectionalLightID dirLightID);

		void SetAmbientLight(const Vec4& ambientLight);
        const Vec4& GetAmbientLight() const;

        void SetSkybox(const SkyboxPtr skybox);
        SkyboxPtr GetSkybox() const;

        Camera& GetSceneCamera();
        const Camera& GetSceneCamera() const;
        SceneNode& GetRootNode();


		const std::string mName;


    private:
        void UpdateDirtyObjects();


        const size_t mHashedID;
        IMemoryAllocator& mMemoryAllocator;
        DataPool<Mat4> mTransformCache;

        std::vector<SceneNode*> mDirtySceneNodes;

		Camera mSceneCamera;
        SceneNode mRootNode;
        Vec4 mAmbientLight;
        SkyboxPtr mSkybox;

        std::vector<ActorPtr> mActors;

		IDMap<PointLight> mPointLights;
        IDMap<DirectionalLight> mDirectionalLights;

        RenderQueue mRenderQueue;
    };

    typedef std::unique_ptr<Scene, std::function<void(Scene*)>> ScenePtr;
}