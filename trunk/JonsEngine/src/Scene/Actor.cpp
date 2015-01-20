#include "include/Scene/Actor.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Actor::Actor(const std::string& name, const ModelPtr model, const SceneNodePtr node) : mName(name), mHashedID(boost::hash_value(name)), mModel(model), mSceneNode(node)
    {
    }
}