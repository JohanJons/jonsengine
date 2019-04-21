#include "Scene/PointLight.h"

namespace JonsEngine
{
    PointLight::PointLight(const std::string& name, const SceneNodeID sceneNodeID) : mName(name), mSceneNodeID(sceneNodeID), mLightColor(1.0f), mLightIntensity(1.0f), mLightRadius(0.0f)
    {
    }


    void PointLight::SetRadius(const float radius)
    {
        mLightRadius = radius;
    }

    void PointLight::SetIntensity(const float intensity)
    {
        mLightIntensity = intensity;
    }

    void PointLight::SetLightColor(const Vec4& lightColor)
    {
        mLightColor = lightColor;
    }


    const std::string& PointLight::GetName() const
    {
        return mName;
    }

    SceneNodeID PointLight::GetSceneNode() const
    {
        return mSceneNodeID;
    }

    const Vec4& PointLight::GetLightColor() const
    {
        return mLightColor;
    }

    float PointLight::GetIntensity() const
    {
        return mLightIntensity;
    }

    float PointLight::GetRadius() const
    {
        return mLightRadius;
    }
}