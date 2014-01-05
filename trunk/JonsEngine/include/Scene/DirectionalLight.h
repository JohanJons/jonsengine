#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Core/Types.h"

#include <string>
#include <memory>

namespace JonsEngine
{
    struct DirectionalLight;

    /* DirectionalLightPtr definition */
    typedef std::shared_ptr<DirectionalLight> DirectionalLightPtr;

    /* DirectionalLight definition */
    struct DirectionalLight
    {
        const std::string mName;
        const size_t mHashedID;
        SceneNodePtr mSceneNode;

        Vec4 mLightColor;
        Vec3 mLightDirection;


        DirectionalLight(const std::string& name, SceneNodePtr node);

        bool operator==(const DirectionalLight& light);
        bool operator==(const std::string& lightName);
    };
}