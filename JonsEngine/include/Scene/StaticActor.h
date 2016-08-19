#pragma once

#include "include/Core/Containers/IDMap.hpp"
#include "include/Scene/BaseActor.h"

namespace JonsEngine
{
    class StaticActor : public BaseActor
    {
    public:
        // TODO: Fix in VS2015+, inheriting constructor not supported in VS2013
        StaticActor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID);
        ~StaticActor();

        // nothing else needed for the moment...
    };

    typedef IDMap<StaticActor>::ItemID StaticActorID;
    static const StaticActorID INVALID_STATIC_ACTOR_ID = IDMapTree<StaticActor>::INVALID_ITEM_ID;
}