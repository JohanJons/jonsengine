#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Resources/Model.h"

#include <string>

namespace JonsEngine
{
    class BaseActor
    {
    public:
        BaseActor(const std::string& name, const ModelID modelId, const SceneNodeID sceneNodeID);
        ~BaseActor();

        void SetModel(const ModelID modelID);
        void SetMaterial(const MaterialID materialID);
        void SetMaterialTilingFactor(const float tilingFactor);
        void SetSceneNode(const SceneNodeID sceneNodeID);

        const std::string& GetName() const;
        ModelID GetModel() const;
        SceneNodeID GetSceneNode() const;
        MaterialID GetMaterial() const;
        float GetMaterialTilingFactor() const;


    protected:
        std::string mName;
        ModelID mModelID;
        SceneNodeID mSceneNodeID;
        MaterialID mMaterialID;
        float mMaterialTilingFactor;
    };
}