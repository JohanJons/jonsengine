#pragma once

#include "include/Scene/Animation.h"
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
        ~Actor();

        void SetAnimation(const AnimationID animationID);
        void SetModel(const ModelID modelID);
        void SetMaterial(const MaterialID materialID);
        void SetMaterialTilingFactor(const float tilingFactor);
        void SetSceneNode(const SceneNodeID sceneNodeID);

        const std::string& GetName() const;
        AnimationID GetAnimation() const;
        ModelID GetModel() const;
        SceneNodeID GetSceneNode() const;
        MaterialID GetMaterial() const;
        float GetMaterialTilingFactor() const;


    private:
        std::string mName;
        AnimationID mAnimationID;
        ModelID mModelID;
        SceneNodeID mSceneNodeID;
        MaterialID mMaterialID;
        float mMaterialTilingFactor;
    };

    typedef IDMap<Actor>::ItemID ActorID;
    static const ActorID INVALID_ACTOR_ID = IDMap<Actor>::INVALID_ITEM_ID;
}