#include "include/Scene/Light.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Light::Light(const std::string& name, LightType type) : mName(name), mHashedID(boost::hash_value(name)), mDiffuseColor(1.0f), mAmbientColor(0.0f), mSpecularColor(0.0f), mLightDirection(1.0f), mLightType(type),
                                                            mConstantAttenutation(0.0f), mLinearAttenuation(1.0f), mQuadraticAttenuation(0.0f), mMaxDistance(50.0f)
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