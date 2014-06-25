#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/Material.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    struct Model;

    /* ModelPtr definition */
    typedef std::shared_ptr<Model> ModelPtr;

    /* Model defintion */
    struct Model
    {
        const std::string mName;
        const size_t mHashedID;

        std::vector<Model> mChildren;
        MeshID mMesh;
        MaterialPtr mMaterial;
        Mat4 mTransform;

        bool mLightingEnabled;
        float mMaterialTilingFactor;

        SceneNodePtr mSceneNode;


        Model(const std::string& name);

        bool operator==(const Model& m);
        bool operator==(const std::string& modelName);
    };
}