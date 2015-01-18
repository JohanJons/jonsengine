#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/Model.h"

#include <memory>
#include <functional>

namespace JonsEngine
{
    class Actor
    {
    public:
        Actor(const std::string& name, const ModelPtr model, const SceneNodePtr node);


        const std::string mName;
        const size_t mHashedID;

        ModelPtr mModel;
        SceneNodePtr mSceneNode;
    };

    typedef std::unique_ptr<Actor, std::function<void(Actor*)>> ActorPtr;
}