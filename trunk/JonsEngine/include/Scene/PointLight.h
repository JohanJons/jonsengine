#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Core/Types.h"

#include <string>
#include <memory>

namespace JonsEngine
{
    struct PointLight;

    /* PointLightPtr definition */
    typedef std::shared_ptr<PointLight> PointLightPtr;

    /* PointLight definition */
    struct PointLight
    {
        const std::string mName;
        const size_t mHashedID;

        Vec4 mLightColor;
        SceneNodePtr mSceneNode;

        float mFalloffFactor;
        float mMaxDistance;


        PointLight(const std::string& name, SceneNodePtr node);

        bool operator==(const PointLight& light);
        bool operator==(const std::string& lightName);
    };
}