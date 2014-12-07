#pragma once

#include "include/Scene/Mesh.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class ModelNode
    {
    public:
        ModelNode(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds);
        ~ModelNode();

        void Transform(const Mat4& transformMatrix);

        const Mat4& GetTransformMatrix() const;
        const Vec3& GetAABBCenter() const;
        const Vec3& GetAABBExtent() const;


        const std::string mName;

        std::vector<Mesh> mMeshes;
        std::vector<ModelNode> mChildNodes;


    private:
        Mat4 mTransform;

        Vec3 mAABBCenter;
        Vec3 mAABBExtent;
    };
}