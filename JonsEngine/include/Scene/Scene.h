#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Scene/PointLight.h"
#include "include/Scene/DirectionalLight.h"
#include "include/Scene/Model.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class IMemoryAllocator;

    class Scene
    {
    public:
        Scene(const std::string& sceneName);
        ~Scene();
        
        PointLightPtr CreatePointLight(const std::string& lightName, const SceneNodePtr node);
        void DeletePointLight(const std::string& lightName);
        PointLightPtr Scene::GetPointLight(const std::string& lightName);
        const std::vector<PointLightPtr>& GetPointLights() const;
        
        DirectionalLightPtr CreateDirectionalLight(const std::string& lightName);
        void DeleteDirectionalLight(const std::string& lightName);
        DirectionalLightPtr Scene::GetDirectionalLight(const std::string& lightName);
        const std::vector<DirectionalLightPtr>& GetDirectionalLights() const;

        void SetAmbientLight(const Vec4& ambientLight);
        const Vec4& GetAmbientLight() const;

        Camera& GetSceneCamera();
        const Camera& GetSceneCamera() const;
        SceneNode& GetRootNode();
        const std::string& GetSceneName() const;

        bool operator==(const Scene& s1);
        bool operator==(const std::string& sceneName);


    private:
        const std::string mName;
        size_t mHashedID;
        Camera mSceneCamera;
        SceneNode mRootNode;
        Vec4 mAmbientLight;

        std::vector<PointLightPtr> mPointLights;
        std::vector<DirectionalLightPtr> mDirectionalLights;

        IMemoryAllocator& mMemoryAllocator;
    };


    /* Scene inlines */
    inline Camera& Scene::GetSceneCamera()                                 { return mSceneCamera;      }
    inline const Camera& Scene::GetSceneCamera() const                     { return mSceneCamera;      }
    inline SceneNode& Scene::GetRootNode()                                 { return mRootNode;         }
    inline const std::string& Scene::GetSceneName() const                  { return mName;             }
}