#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/functional/hash.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/bind.hpp"
#include <algorithm>

namespace JonsEngine
{
    Scene::Scene(const std::string& sceneName) : mName(sceneName), mHashedID(boost::hash_value(sceneName)), mRootNode("Root"), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mAmbientLight(1.0f)
    {
    }

    Scene::~Scene()
    {
    }


    EntityPtr Scene::CreateEntity(const std::string& entityName)
    {
        EntityPtr entity(mMemoryAllocator.AllocateObject<Entity>(entityName),
                           boost::bind(&HeapAllocator::DeallocateObject<Entity>, &mMemoryAllocator, _1));

        mEntities.push_back(entity);

        return entity;
    }
        
    EntityPtr Scene::GetEntity(const std::string& entityName)
    {
        EntityPtr ret;
        std::vector<EntityPtr>::iterator iter = std::find_if(mEntities.begin(), mEntities.end(), boost::bind(&Entity::mName, _1) == entityName);

        if (iter != mEntities.end())
            ret = * iter;

        return ret;
    }

    const std::vector<EntityPtr>& Scene::GetAllEntities() const
    {
        return mEntities;
    }
        
    void Scene::DeleteEntity(const std::string& entityName)
    {
        std::vector<EntityPtr>::iterator iter = std::find_if(mEntities.begin(), mEntities.end(), boost::bind(&Entity::mName, _1) == entityName);

        if (iter != mEntities.end())
            mEntities.erase(iter);
    }


    LightPtr Scene::CreateLight(const std::string& lightName)
    {
        LightPtr light(mMemoryAllocator.AllocateObject<Light>(lightName),
                           boost::bind(&HeapAllocator::DeallocateObject<Light>, &mMemoryAllocator, _1));

        mLights.push_back(light);

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

    const Scene::LightColor& Scene::GetAmbientLight() const
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