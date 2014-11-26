#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/EngineDefs.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/functional/hash.hpp"
#include <algorithm>

namespace JonsEngine
{
    Scene::Scene(const std::string& sceneName) : mName(sceneName), mHashedID(boost::hash_value(sceneName)), mRootNode("Root"), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), 
                                                                                mAmbientLight(0.2f)
    {
    }

    Scene::~Scene()
    {
    }



    PointLightPtr Scene::CreatePointLight(const std::string& lightName, const SceneNodePtr node)
    {
        mPointLights.emplace_back(mMemoryAllocator.AllocateObject<PointLight>(lightName, node), std::bind(&HeapAllocator::DeallocateObject<PointLight>, &mMemoryAllocator, std::placeholders::_1));

        return mPointLights.back();
    }

    DirectionalLightPtr Scene::CreateDirectionalLight(const std::string& lightName)
    {
        mDirectionalLights.emplace_back(mMemoryAllocator.AllocateObject<DirectionalLight>(lightName), std::bind(&HeapAllocator::DeallocateObject<DirectionalLight>, &mMemoryAllocator, std::placeholders::_1));

        return mDirectionalLights.back();
    }


    PointLightPtr Scene::GetPointLight(const std::string& lightName)
    {
        PointLightPtr ret;
        size_t hashedID = boost::hash_value(lightName);

        auto iter = std::find_if(mPointLights.begin(), mPointLights.end(), [hashedID](const PointLightPtr light) { return light->mHashedID == hashedID; });

        if (iter != mPointLights.end())
            ret = *iter;

        return ret;
    }

    DirectionalLightPtr Scene::GetDirectionalLight(const std::string& lightName)
    {
        DirectionalLightPtr ret;
        size_t hashedID = boost::hash_value(lightName);

        auto iter = std::find_if(mDirectionalLights.begin(), mDirectionalLights.end(), [hashedID](const DirectionalLightPtr light) { return light->mHashedID == hashedID; });

        if (iter != mDirectionalLights.end())
            ret = *iter;

        return ret;
    }


    const std::vector<PointLightPtr>& Scene::GetPointLights() const
    {
        return mPointLights;
    }

    const std::vector<DirectionalLightPtr>& Scene::GetDirectionalLights() const
    {
        return mDirectionalLights;
    }

    void Scene::DeletePointLight(const std::string& lightName)
    {
        size_t hashedID = boost::hash_value(lightName);
        auto iter = std::find_if(mPointLights.begin(), mPointLights.end(), [hashedID](const PointLightPtr light) { return light->mHashedID == hashedID; });

        if (iter != mPointLights.end())
            mPointLights.erase(iter);
    }

    void Scene::DeleteDirectionalLight(const std::string& lightName)
    {
        size_t hashedID = boost::hash_value(lightName);
        auto iter = std::find_if(mDirectionalLights.begin(), mDirectionalLights.end(), [hashedID](const DirectionalLightPtr light) { return light->mHashedID == hashedID; });

        if (iter != mDirectionalLights.end())
            mDirectionalLights.erase(iter);
    }


    void Scene::SetAmbientLight(const Vec4& ambientLight)
    {
        mAmbientLight = ambientLight;
    }

    const Vec4& Scene::GetAmbientLight() const
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