#pragma once

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

        Vec4 mLightColor;
        Vec3 mLightDirection;


        DirectionalLight(const std::string& name);

        bool operator==(const DirectionalLight& light);
        bool operator==(const std::string& lightName);
    };
}