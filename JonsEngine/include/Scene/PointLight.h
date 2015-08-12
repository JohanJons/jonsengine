#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

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
        SceneNodePtr mSceneNode;

        Vec4 mLightColor;
        float mLightIntensity;
        float mLightRadius;


    private:
        const size_t mHashedID;
    };

    typedef IDMap<PointLight>::ItemID PointLightID;
    static const PointLightID INVALID_POINT_LIGHT_ID = IDMap<PointLight>::INVALID_ITEM_ID;
}