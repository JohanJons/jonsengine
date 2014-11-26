#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/Material.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>
#include <memory>

namespace JonsEngine
{
    class Model
    {
    public:
        Model(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds);
        ~Model();

        bool operator==(const Model& m);
        bool operator==(const std::string& modelName);


        const std::string mName;
        const size_t mHashedID;

        std::vector<Model> mChildren;
        MeshID mMesh;
        MaterialPtr mMaterial;
        Mat4 mTransform;
        const Vec3 mAABBCenter;
        const Vec3 mAABBExtent;

        bool mLightingEnabled;
        float mMaterialTilingFactor;

        SceneNodePtr mSceneNode;
    };

    typedef std::shared_ptr<Model> ModelPtr;
}