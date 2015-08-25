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

        // for now only applies material to all meshes
        // TODO: explicit mapping of mesh/material to only apply material to certain mesh(es)
        void SetMaterial(const MaterialID materialID);
        MaterialID GetMaterial() const;

        void SetMaterialTilingFactor(const float tilingFactor);
        float GetMaterialTilingFactor() const;


        const std::string mName;
        const ModelID mModelID;
        const SceneNodePtr mSceneNode;


    private:
        MaterialID mModelMaterial;
        float mMaterialTilingFactor;
    };

    typedef IDMap<Actor>::ItemID ActorID;
    static const ActorID INVALID_ACTOR_ID = IDMap<Actor>::INVALID_ITEM_ID;
}