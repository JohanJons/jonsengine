#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Scene/Light.h"
#include "include/Scene/Model.h"
#include "include/Resources/JonsPackage.h"
#include "include/Resources/ResourceManifest.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class IMemoryAllocator;

    /* Scene definition */
    class Scene
    {
    public:
        typedef ResourceManifest::ShapeType Shape;

        Scene(const std::string& sceneName, ResourceManifest& resManifest);
        ~Scene();

        LightPtr CreateLight(const std::string& lightName, Light::LightType type);
        LightPtr CreateLight(const std::string& lightName, Light::LightType type, const SceneNodePtr node);
        LightPtr GetLight(const std::string& lightName);
        const std::vector<LightPtr>& GetAllLights() const;
        void DeleteLight(const std::string& lightName);

        void SetGammaFactor(const float gamma);
        float GetGammaFactor() const;
        Vec4 GetGamma() const;

        const ResourceManifest& GetResourceManifest() const;
        ResourceManifest& GetResourceManifest();

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
        float mGamma;

        std::vector<LightPtr> mLights;

        ResourceManifest& mResourceManifest;
        IMemoryAllocator& mMemoryAllocator;
    };


    /* Scene inlines */
    inline const ResourceManifest& Scene::GetResourceManifest() const      { return mResourceManifest; }
    inline ResourceManifest& Scene::GetResourceManifest()                  { return mResourceManifest; }
    inline Camera& Scene::GetSceneCamera()                                 { return mSceneCamera;      }
    inline const Camera& Scene::GetSceneCamera() const                     { return mSceneCamera;      }
    inline SceneNode& Scene::GetRootNode()                                 { return mRootNode;         }
    inline const std::string& Scene::GetSceneName() const                  { return mName;             }
}