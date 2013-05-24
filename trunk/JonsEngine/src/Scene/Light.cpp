#include "include/Scene/Light.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Light::Light(const std::string& name, LightType type) : mName(name), mHashedID(boost::hash_value(name)), mLightColor(1.0f), mLightDirection(1.0f), mLightType(type),
                                                            mIntensity(1.0f), mRadius(0.0f), mMaxDistance(0.0f)
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