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
            POINT = 0,
            DIRECTIONAL
        };

        const std::string mName;
        const size_t mHashedID;

        Vec4 mDiffuseColor;
        Vec4 mAmbientColor;
        Vec4 mSpecularColor;

        SceneNodePtr mSceneNode;
        Vec3 mLightDirection;
        LightType mLightType;

        float mConstantAttenutation;
        float mLinearAttenuation;
        float mQuadraticAttenuation;
        float mMaxDistance;



        Light(const std::string& name, LightType type);

        bool operator==(const Light& light);
        bool operator==(const std::string& lightName);
    };
}