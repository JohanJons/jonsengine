#include "include/Scene/DirectionalLight.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    DirectionalLight::DirectionalLight(const std::string& name, SceneNodePtr node) : mName(name), mHashedID(boost::hash_value(name)), mSceneNode(node), mLightColor(1.0f), mLightDirection(0.0f, 0.0f, -1.0f)
    {
    }


    bool DirectionalLight::operator==(const DirectionalLight& light)
    {
        return mHashedID == light.mHashedID;
    }

    bool DirectionalLight::operator==(const std::string& lightName)
    {
        return mHashedID == boost::hash_value(lightName);
    }
}