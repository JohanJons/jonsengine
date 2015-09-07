#include "include/Scene/PointLight.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    PointLight::PointLight(const std::string& name, const SceneNodeID sceneNodeID) : mName(name), mHashedID(boost::hash_value(name)), mSceneNodeID(sceneNodeID), mLightColor(1.0f), mLightIntensity(1.0f), mLightRadius(0.0f)
    {
    }


    bool PointLight::operator==(const PointLight& light)
    {
        return mHashedID == light.mHashedID;
    }

    bool PointLight::operator==(const std::string& lightName)
    {
        return mHashedID == boost::hash_value(lightName);
    }
}