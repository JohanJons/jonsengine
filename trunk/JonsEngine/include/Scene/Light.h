#pragma once

#include "include/Core/Types.h"
#include "include/Scene/SceneNode.h"

#include <string>
#include <memory>

namespace JonsEngine
{
    struct Light;

    /* LightPtr definition */
    typedef std::shared_ptr<Light> LightPtr;

    /* Light definition */
    struct Light
    {
        const std::string mName;
        const size_t mHashedID;

        Vec4 mLightColor;
        SceneNodePtr mSceneNode;
        Vec3 mLightDirection;
        LightType mLightType;

        float mIntensity;
        float mRadius;
        float mMaxDistance;


        Light(const std::string& name, LightType type);

        bool operator==(const Light& light);
        bool operator==(const std::string& lightName);
    };
}