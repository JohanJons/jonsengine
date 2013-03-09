#pragma once

#include "include/Scene/Model.h"
#include "include/Scene/SceneNode.h"

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
        SceneNode* mNode;

        Entity(const std::string& name);

        bool operator==(const Entity& entity);
        bool operator==(const std::string& entityName);
    };
}