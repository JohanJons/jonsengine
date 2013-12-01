#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineDefs.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/functional/hash.hpp"
#include <algorithm>

namespace JonsEngine
{
    Scene::Scene(const std::string& sceneName, ResourceManifest& resManifest) : mName(sceneName), mHashedID(boost::hash_value(sceneName)), mRootNode("Root"), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), 
                                                                                mGamma(2.1f), mResourceManifest(resManifest)
    {
    }

    Scene::~Scene()
    {
    }


    LightPtr Scene::CreateLight(const std::string& lightName, LightType type)
    {
        mLights.emplace_back(mMemoryAllocator.AllocateObject<Light>(lightName, type), std::bind(&HeapAllocator::DeallocateObject<Light>, &mMemoryAllocator, std::placeholders::_1));

        return mLights.back();
    }

    LightPtr Scene::CreateLight(const std::string& lightName, LightType type, const SceneNodePtr node)
    {
        LightPtr light = CreateLight(lightName, type);

        if (light)
            light->mSceneNode = node;

        return light;
    }
        
    LightPtr Scene::GetLight(const std::string& lightName)
    {
        LightPtr ret;
        size_t hashedID = boost::hash_value(lightName);

        auto iter = std::find_if(mLights.begin(), mLights.end(), [hashedID](const LightPtr light) { return light->mHashedID == hashedID; });

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
        size_t hashedID = boost::hash_value(lightName);
        auto iter = std::find_if(mLights.begin(), mLights.end(), [hashedID](const LightPtr light) { return light->mHashedID == hashedID; });

        if (iter != mLights.end())
            mLights.erase(iter);
    }


    void Scene::SetGammaFactor(const float gamma)
    {
        mGamma = gamma;
    }
        
    float Scene::GetGammaFactor() const
    {
        return mGamma;
    }

    Vec4 Scene::GetGamma() const
    {
        Vec4 gamma(1.0f / mGamma);
        gamma.w = 1.0f;

        return gamma;
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