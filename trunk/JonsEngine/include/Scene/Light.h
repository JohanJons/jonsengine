#pragma once

#include "include/Core/Types.h"
#include "include/Scene/SceneNode.h"

#include "boost/smart_ptr.hpp"
#include <string>

namespace JonsEngine
{
    struct Light;

    /* LightPtr definition */
    typedef boost::shared_ptr<Light> LightPtr;

    /* Light definition */
    struct Light
    {
        enum LightType
        {
            POSITIONAL = 0,
            DIRECTIONAL
        };

        const std::string mName;
        const size_t mHashedID;

        Vec4 mLightIntensity;
        Vec3 mLightDirection;
        float mLightAttenuation;
        LightType mLightType;

        SceneNodePtr mSceneNode;


        Light(const std::string& name, LightType type);

        bool operator==(const Light& light);
        bool operator==(const std::string& lightName);
    };
}