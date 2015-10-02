#include "include/Scene/Actor.h"

namespace JonsEngine
{
    Actor::Actor(const std::string& name, const ModelID model, const SceneNodeID sceneNodeID) : mName(name), mModelID(model), mSceneNodeID(sceneNodeID), mMaterialID(INVALID_MATERIAL_ID), mMaterialTilingFactor(1.0f)
    {
    }

    Actor::~Actor()
    {
    }


    void Actor::SetAnimation(const AnimationID animationID)
    {

    }

    void Actor::SetModel(const ModelID modelID)
    {
        mModelID = modelID;
    }

    void Actor::SetMaterial(const MaterialID materialID)
    {
        mMaterialID = materialID;
    }

    void Actor::SetMaterialTilingFactor(const float tilingFactor)
    {
        mMaterialTilingFactor = tilingFactor;
    }

    void Actor::SetSceneNode(const SceneNodeID sceneNodeID)
    {
        mSceneNodeID = sceneNodeID;
    }


    const std::string& Actor::GetName() const
    {
        return mName;
    }

    ModelID Actor::GetModel() const
    {
        return mModelID;
    }

    SceneNodeID Actor::GetSceneNode() const
    {
        return mSceneNodeID;
    }

    MaterialID Actor::GetMaterial() const
    {
        return mMaterialID;
    }

    float Actor::GetMaterialTilingFactor() const
    {
        return mMaterialTilingFactor;
    }
}