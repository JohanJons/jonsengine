#pragma once

#include "SceneNode.h"
#include "Types.h"
#include "IDMap.hpp"

#include <string>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class PointLight
    {
    public:
        PointLight(const std::string& name, const SceneNodeID sceneNodeID);

        void SetRadius(const float radius);
        void SetIntensity(const float intensity);
        void SetLightColor(const Vec4& lightColor);

        const std::string& GetName() const;
        SceneNodeID GetSceneNode() const;
        const Vec4& GetLightColor() const;
        float GetIntensity() const;
        float GetRadius() const;


    private:
        std::string mName;
        SceneNodeID mSceneNodeID;

        Vec4 mLightColor;
        float mLightIntensity;
        float mLightRadius;
    };

    typedef IDMap<PointLight>::ItemID PointLightID;
    static const PointLightID INVALID_POINT_LIGHT_ID = IDMap<PointLight>::INVALID_ITEM_ID;
}