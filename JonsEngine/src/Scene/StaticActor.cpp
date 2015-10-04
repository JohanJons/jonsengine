#include "include/Scene/StaticActor.h"

namespace JonsEngine
{
    StaticActor::StaticActor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID) : BaseActor(name, modelId, sceneNodeID)
    {
    }

    StaticActor::~StaticActor()
    {
    }
}