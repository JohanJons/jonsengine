#include "include/Scene/Scene.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/functional/hash.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/bind.hpp"
#include <algorithm>

namespace JonsEngine
{
    Scene::Scene(const std::string& sceneName) : mName(sceneName), mHashedID(boost::hash_value(sceneName)), mRootNode("Root"), mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator())
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
        return *std::find_if(mEntities.begin(), mEntities.end(), boost::bind(&Entity::mName, _1) == entityName);
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


    bool Scene::operator==(const Scene& s1)
    {
        return mHashedID == s1.mHashedID;
    }

    bool Scene::operator==(const std::string& sceneName)
    {
        return mHashedID == boost::hash_value(sceneName);
    }
}