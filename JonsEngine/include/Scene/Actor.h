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
        Actor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID);
        Actor& operator=(Actor&& otherActor)
        {
            if (this != &otherActor)
            {
                mName = std::move(otherActor.mName);
                mModelID = std::move(otherActor.mModelID);
                mSceneNodeID = std::move(otherActor.mSceneNodeID);
                mMaterialID = std::move(otherActor.mMaterialID);
                mMaterialTilingFactor = std::move(otherActor.mMaterialTilingFactor);

                otherActor.mModelID = INVALID_MODEL_ID;
                otherActor.mSceneNodeID = INVALID_SCENE_NODE_ID;
                otherActor.mMaterialID = INVALID_MATERIAL_ID;
            }

            return *this;
        }
        ~Actor();


        std::string mName;
        ModelID mModelID;
        SceneNodeID mSceneNodeID;

        MaterialID mMaterialID;
        float mMaterialTilingFactor;
    };

    typedef IDMap<Actor>::ItemID ActorID;
    static const ActorID INVALID_ACTOR_ID = IDMap<Actor>::INVALID_ITEM_ID;
}