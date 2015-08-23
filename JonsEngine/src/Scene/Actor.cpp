#include "include/Scene/Actor.h"

namespace JonsEngine
{
    Actor::Actor(const std::string& name, const ModelID model, const SceneNodePtr node) : mName(name), mModelID(model), mSceneNode(node), mMaterialTilingFactor(1.0f)
    {
    }

    Actor::~Actor()
    {
    }


    void Actor::SetMaterialTilingFactor(const float tilingFactor)
    {
        mMaterialTilingFactor = tilingFactor;
    }

    float Actor::GetMaterialTilingFactor() const
    {
        return mMaterialTilingFactor;
    }
}