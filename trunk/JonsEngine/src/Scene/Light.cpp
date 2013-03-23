#include "include/Scene/Light.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Light::Light(const std::string& name) : mName(name), mHashedID(boost::hash_value(name)), mLightIntensity(1.0f, 1.0f, 1.0f, 1.0f), mLightDirection(1.0f, 1.0f, 1.0f)
    {
    }

        
    bool Light::operator==(const Light& light)
    {
        return mHashedID == light.mHashedID;
    }
        
    bool Light::operator==(const std::string& lightName)
    {
        return mHashedID == boost::hash_value(lightName);
    }
}