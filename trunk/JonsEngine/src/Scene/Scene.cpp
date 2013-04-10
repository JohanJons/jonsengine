#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineDefs.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/functional/hash.hpp"
#include "boost/bind.hpp"
#include <algorithm>

namespace JonsEngine
{
    Scene::Scene(const std::string& sceneName, ResourceManifest& resManifest) : mName(sceneName), mHashedID(boost::hash_value(sceneName)), mRootNode("Root"), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mAmbientLight(1.0f),
                                                                                mResourceManifest(resManifest)
    {
    }

    Scene::~Scene()
    {
    }


    ModelPtr Scene::CreateModel(const std::string& modelName, const std::string& assetName, JonsPackagePtr jonsPkg)
    {
        ModelPtr model = mResourceManifest.LoadModel(modelName, assetName, jonsPkg);

        if (model)
            mModels.push_back(model);

        return model;
    }

    ModelPtr Scene::CreateModel(const std::string& modelName, const std::string& assetName, JonsPackagePtr jonsPkg, const SceneNodePtr node)
    {
        ModelPtr model    = CreateModel(modelName, assetName, jonsPkg);
        model->mSceneNode = node;

        return model;
    }
        
    ModelPtr Scene::GetModel(const std::string& modelName)
    {
        ModelPtr ret;
        std::vector<ModelPtr>::iterator iter = std::find_if(mModels.begin(), mModels.end(), boost::bind(&Model::mName, _1) == modelName);

        if (iter != mModels.end())
            ret = * iter;

        return ret;
    }
        
    const std::vector<ModelPtr>& Scene::GetAllModels() const
    {
        return mModels;
    }
        
    void Scene::DeleteModel(const std::string& modelName)
    {
        std::vector<ModelPtr>::iterator iter = std::find_if(mModels.begin(), mModels.end(), boost::bind(&Model::mName, _1) == modelName);

        if (iter != mModels.end())
            mModels.erase(iter);
    }


    LightPtr Scene::CreateLight(const std::string& lightName, Light::LightType type)
    {
        LightPtr light(mMemoryAllocator.AllocateObject<Light>(lightName, type),
                           boost::bind(&HeapAllocator::DeallocateObject<Light>, &mMemoryAllocator, _1));

        mLights.push_back(light);

        return light;
    }

    LightPtr Scene::CreateLight(const std::string& lightName, Light::LightType type, const SceneNodePtr node)
    {
        LightPtr light    = CreateLight(lightName, type);
        light->mSceneNode = node;

        return light;
    }
        
    LightPtr Scene::GetLight(const std::string& lightName)
    {
        LightPtr ret;
        std::vector<LightPtr>::iterator iter = std::find_if(mLights.begin(), mLights.end(), boost::bind(&Light::mName, _1) == lightName);

        if (iter != mLights.end())
            ret = * iter;

        return ret;
    }

    const std::vector<LightPtr>& Scene::GetAllLights() const
    {
        return mLights;
    }
        
    void Scene::DeleteLight(const std::string& lightName)
    {
        std::vector<LightPtr>::iterator iter = std::find_if(mLights.begin(), mLights.end(), boost::bind(&Light::mName, _1) == lightName);

        if (iter != mLights.end())
            mLights.erase(iter);
    }


    void Scene::SetAmbientLight(const LightColor& ambientLight)
    {
        mAmbientLight = ambientLight;
    }

    const LightColor& Scene::GetAmbientLight() const
    {
        return mAmbientLight;
    }


    bool Scene::operator==(const Scene& s1)
    {
        return mHashedID == s1.mHashedID;
    }

    bool Scene::operator==(const std::string& sceneName)
    {
        return mHashedID == boost::hash_value(sceneName);
    }
}