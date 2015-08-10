#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Resources/Model.h"

#include <string>

namespace JonsEngine
{
    class Actor
    {
    public:
        Actor(const std::string& name, const ModelID modelId, const SceneNodePtr node);
        ~Actor();


        const std::string mName;

        ModelID mModelID;
        SceneNodePtr mSceneNode;
    };
}