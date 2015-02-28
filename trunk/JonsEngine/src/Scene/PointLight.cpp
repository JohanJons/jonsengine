#include "include/Scene/PointLight.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    PointLight::PointLight(const std::string& name, const OnChangeSceneNodeFunc& onChangeSceneNode, const OnDirtyFunc& onDirty) :
        mName(name), mHashedID(boost::hash_value(name)), mSceneNode(nullptr), mLightColor(1.0f), mLightIntensity(1.0f), mLightRadius(1.0f), mOnChangeSceneNode(onChangeSceneNode), mOnDirty(onDirty)
    {
    }


    bool PointLight::operator==(const PointLight& light)
    {
        return mHashedID == light.mHashedID;
    }

    bool PointLight::operator==(const std::string& lightName)
    {
        return mHashedID == boost::hash_value(lightName);
    }


    void PointLight::SetSceneNode(SceneNodePtr sceneNode)
    {
        mSceneNode = sceneNode;

        mOnChangeSceneNode(this, mSceneNode.get());
        mOnDirty(this);
    }

    SceneNodePtr PointLight::GetSceneNode()
    {
        return mSceneNode;
    }

    void PointLight::SetLightColor(const Vec4& color)
    {
        mLightColor = color;

        mOnDirty(this);
    }

    const Vec4& PointLight::GetLightColor() const
    {
        return mLightColor;
    }


    void PointLight::SetLightIntensity(const float intensity)
    {
        mLightIntensity = intensity;

        mOnDirty(this);
    }

    float PointLight::GetLightIntensity() const
    {
        return mLightIntensity;
    }


    void PointLight::SetLightRadius(const float radius)
    {
        mLightRadius = radius;

        mOnDirty(this);
    }

    float PointLight::GetLightRadius() const
    {
        return mLightRadius;
    }
}