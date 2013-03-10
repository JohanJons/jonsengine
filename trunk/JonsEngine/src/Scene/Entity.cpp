#include "include/Scene/Entity.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Entity::Entity(const std::string& name) : mName(name), mHashedID(boost::hash_value(name)), mRender(true)
    {
    }

    bool Entity::operator==(const Entity& entity)
    {
         return mHashedID == entity.mHashedID;
    }
       
    bool Entity::operator==(const std::string& entityName)
    {
        return mHashedID == boost::hash_value(entityName);
    }
}