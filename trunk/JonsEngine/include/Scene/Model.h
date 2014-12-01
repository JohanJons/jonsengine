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
        Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds);
        ~Model();

        Mat4 Transform(const Mat4& transformationMatrix);

        const Mat4& GetTransformMatrix() const;
        const Vec3& GetAABBCenter() const;
        const Vec3& GetAABBExtent() const;

        bool operator==(const Model& m);
        bool operator==(const std::string& modelName);


        const std::string mName;
        const size_t mHashedID;

        std::vector<Model> mChildren;
        MeshID mMesh;
        MaterialPtr mMaterial;

        bool mLightingEnabled;
        float mMaterialTilingFactor;

        SceneNodePtr mSceneNode;

        
    private:
        Mat4 mTransform;
        Vec3 mAABBCenter;
        Vec3 mAABBExtent;
    };

    typedef std::shared_ptr<Model> ModelPtr;
}