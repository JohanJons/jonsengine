#include "include/Scene/Scene.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Scene::Scene(const std::string& sceneName) : mName(sceneName), mHashedID(boost::hash_value(sceneName)), mRootNode("Root")
    {
    }

    Scene::~Scene()
    {
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