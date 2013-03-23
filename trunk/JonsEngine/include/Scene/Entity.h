#pragma once

#include "include/Scene/Model.h"
#include "include/Scene/SceneNode.h"
#include "include/Scene/Light.h"

#include <string>

namespace JonsEngine
{
    struct Entity;

    /* EntityPtr definition */
    typedef boost::shared_ptr<Entity> EntityPtr;

    /* Entity definition */
    struct Entity
    {
        const std::string mName;
        const size_t mHashedID;
        bool mRender;

        ModelPtr mModel;
        LightPtr mLight;
        SceneNodePtr mNode;

        Entity(const std::string& name);

        bool operator==(const Entity& entity);
        bool operator==(const std::string& entityName);
    };
}