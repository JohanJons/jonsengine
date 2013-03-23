#pragma once

#include "include/Core/Types.h"

#include "boost/smart_ptr.hpp"
#include <string>

namespace JonsEngine
{
    struct Light;

    /* EntityPtr definition */
    typedef boost::shared_ptr<Light> LightPtr;

    struct Light
    {
        const std::string mName;
        const size_t mHashedID;

        Vec4 mLightIntensity;
        Vec3 mLightDirection;

        Light(const std::string& name);

        bool operator==(const Light& light);
        bool operator==(const std::string& lightName);
    };
}