#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Core/Types.h"

#include <string>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class PointLight
    {
    public:
        PointLight(const std::string& name, SceneNodePtr node);

        bool operator==(const PointLight& light);
        bool operator==(const std::string& lightName);


        const std::string mName;
        const size_t mHashedID;
        SceneNodePtr mSceneNode;

        Vec4 mLightColor;
        float mLightIntensity;
        float mMaxDistance;
    };

    typedef std::unique_ptr<PointLight, std::function<void(PointLight*)>> PointLightPtr;
}