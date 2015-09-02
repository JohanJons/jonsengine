#include "include/Scene/Actor.h"

namespace JonsEngine
{
    Actor::Actor(const std::string& name, const ModelID model, const SceneNodeID sceneNodeID) : mName(name), mModelID(model), mSceneNodeID(sceneNodeID), mMaterialID(INVALID_MATERIAL_ID), mMaterialTilingFactor(1.0f)
    {
    }

    Actor::~Actor()
    {
    }
}