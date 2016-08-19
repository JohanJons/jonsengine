#include "include/Scene/BaseActor.h"

namespace JonsEngine
{
    BaseActor::BaseActor(const std::string& name, const ModelID model, const SceneNodeID sceneNodeID) : mName(name), mModelID(model), mSceneNodeID(sceneNodeID), mMaterialID(INVALID_MATERIAL_ID), mMaterialTilingFactor(1.0f)
    {
    }

    BaseActor::~BaseActor()
    {
    }


    void BaseActor::SetModel(const ModelID modelID)
    {
        mModelID = modelID;
    }

    void BaseActor::SetMaterial(const MaterialID materialID)
    {
        mMaterialID = materialID;
    }

    void BaseActor::SetMaterialTilingFactor(const float tilingFactor)
    {
        mMaterialTilingFactor = tilingFactor;
    }

    void BaseActor::SetSceneNode(const SceneNodeID sceneNodeID)
    {
        mSceneNodeID = sceneNodeID;
    }


    const std::string& BaseActor::GetName() const
    {
        return mName;
    }

    ModelID BaseActor::GetModel() const
    {
        return mModelID;
    }

    SceneNodeID BaseActor::GetSceneNode() const
    {
        return mSceneNodeID;
    }

    MaterialID BaseActor::GetMaterial() const
    {
        return mMaterialID;
    }

    float BaseActor::GetMaterialTilingFactor() const
    {
        return mMaterialTilingFactor;
    }
}