#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/Camera.h"
#include "include/Scene/Light.h"
#include "include/Scene/Model.h"
#include "include/Resources/JonsPackage.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class IMemoryAllocator;
    class ResourceManifest;

    /* Scene definition */
    class Scene
    {
    public:
        Scene(const std::string& sceneName, ResourceManifest& resManifest);
        ~Scene();

        ModelPtr CreateModel(const std::string& modelName, const std::string& assetName, JonsPackagePtr jonsPkg);
        ModelPtr CreateModel(const std::string& modelName, const std::string& assetName, JonsPackagePtr jonsPkg, const SceneNodePtr node);
        ModelPtr GetModel(const std::string& modelName);
        const std::vector<ModelPtr>& GetAllModels() const;
        void DeleteModel(const std::string& modelName);

        LightPtr CreateLight(const std::string& lightName, Light::LightType type);
        LightPtr CreateLight(const std::string& lightName, Light::LightType type, const SceneNodePtr node);
        LightPtr GetLight(const std::string& lightName);
        const std::vector<LightPtr>& GetAllLights() const;
        void DeleteLight(const std::string& lightName);

        void SetAmbientLight(const LightColor& ambientLight);
        const LightColor& GetAmbientLight() const;
        void SetGammaFactor(const float gamma);
        float GetGammaFactor() const;
        Vec4 GetGamma() const;

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

        LightColor mAmbientLight;
        float mGamma;

        std::vector<ModelPtr> mModels;
        std::vector<LightPtr> mLights;

        ResourceManifest& mResourceManifest;
        IMemoryAllocator& mMemoryAllocator;
    };


    /* Scene inlines */
    inline Camera& Scene::GetSceneCamera()                     { return mSceneCamera;  }
    inline const Camera& Scene::GetSceneCamera() const         { return mSceneCamera;  }
    inline SceneNode& Scene::GetRootNode()                     { return mRootNode;     }
    inline const std::string& Scene::GetSceneName() const      { return mName;         }
}