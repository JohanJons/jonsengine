#pragma once

#include "include/Core/Containers/IDMap.h"
#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Scene/PointLight.h"
#include "include/Scene/DirectionalLight.h"
#include "include/Scene/Model.h"
#include "include/Scene/Actor.h"
#include "include/Renderer/RenderQueue.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class IMemoryAllocator;

    typedef IDMap<PointLight>::ItemID PointLightID;

    class Scene
    {
    public:
		Scene(const std::string& sceneName);
        ~Scene();

        bool operator==(const Scene& s1);
        bool operator==(const std::string& sceneName);

        const RenderQueue& GetRenderQueue(const uint32_t windowWidth, const uint32_t windowHeight, const float zNear, const float zFar);

        Actor* CreateActor(const std::string& actorName, const ModelPtr model, const SceneNodePtr node);
        void DeleteActor(Actor* actor);
        Actor* GetActor(const std::string& actorName);
		const std::vector<ActorPtr>& GetActors() const;
        
		PointLightID CreatePointLight(const std::string& lightName, SceneNodePtr node);
        void DeletePointLight(const PointLightID pointLightID);
		PointLight& GetPointLight(const PointLightID pointLightID);
        
        DirectionalLight* CreateDirectionalLight(const std::string& lightName);
        DirectionalLight* CreateDirectionalLight(const std::string& lightName, const uint32_t numShadowmapCascades);
        void DeleteDirectionalLight(const DirectionalLight* dirLight);
        DirectionalLight* GetDirectionalLight(const std::string& lightName);
        const std::vector<DirectionalLightPtr>& GetDirectionalLights() const;

		void SetAmbientLight(const Vec4& ambientLight);
        const Vec4& GetAmbientLight() const;

        Camera& GetSceneCamera();
        const Camera& GetSceneCamera() const;
        SceneNode& GetRootNode();


		const std::string mName;


    private:
        void UpdateDirtyObjects();


        const size_t mHashedID;
        IMemoryAllocator& mMemoryAllocator;

        std::vector<SceneNode*> mDirtySceneNodes;

		Camera mSceneCamera;
        SceneNode mRootNode;
        Vec4 mAmbientLight;

        std::vector<ActorPtr> mActors;
        std::vector<DirectionalLightPtr> mDirectionalLights;

		IDMap<PointLight> mPointLights;

        RenderQueue mRenderQueue;
    };

    typedef std::unique_ptr<Scene, std::function<void(Scene*)>> ScenePtr;
}