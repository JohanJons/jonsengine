#pragma once

#include "include/Scene/SceneNode.h"

#include <string>

namespace JonsEngine
{
    class Actor
    {
    public:
        Actor(const std::string& name, const ModelPtr model, const SceneNodePtr node);
        ~Actor();


        const std::string mName;

        ModelPtr mModel;
        SceneNodePtr mSceneNode;
    };
}