#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/Material.h"
#include "include/Core/Types.h"
#include "include/Renderer/IMesh.h"
#include "include/Renderer/ITexture.h"

#include "boost/smart_ptr.hpp"
#include <string>
#include <vector>

namespace JonsEngine
{
    struct Model;

    /* ModelPtr definition */
    typedef boost::shared_ptr<Model> ModelPtr;

    /* Model defintion */
    struct Model
    {
        const std::string mName;
        const size_t mHashedID;

        std::vector<Model> mChildren;
        MeshPtr mMesh;
        MaterialPtr mMaterial;
        Mat4 mTransform;
        bool mLightingEnabled;

        SceneNodePtr mSceneNode;

        Model(const std::string& name);

        bool operator==(const Model& m);
        bool operator==(const std::string& modelName);
    };
}