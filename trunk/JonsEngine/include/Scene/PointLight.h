#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Core/Types.h"

#include <string>
#include <memory>

namespace JonsEngine
{
    struct PointLight;

    typedef std::shared_ptr<PointLight> PointLightPtr;

    struct PointLight
    {
        const std::string mName;
        const size_t mHashedID;
        SceneNodePtr mSceneNode;

        Vec4 mLightColor;
        float mLightIntensity;
        float mMaxDistance;


        PointLight(const std::string& name, SceneNodePtr node);

        bool operator==(const PointLight& light);
        bool operator==(const std::string& lightName);
    };
}