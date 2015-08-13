#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Resources/Model.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>

namespace JonsEngine
{
    class Actor
    {
    public:
        Actor(const std::string& name, const ModelID modelId, const SceneNodePtr node);
        ~Actor();

        void SetMaterialTilingFactor(const float tilingFactor);
        float GetMaterialTilingFactor() const;


        const std::string mName;
        const ModelID mModelID;
        const SceneNodePtr mSceneNode;


    private:
        float mMaterialTilingFactor;
    };

    typedef IDMap<Actor>::ItemID ActorID;
    static const ActorID INVALID_ACTOR_ID = IDMap<Actor>::INVALID_ITEM_ID;
}