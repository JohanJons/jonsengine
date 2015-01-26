#pragma once

#include "include/Scene/Mesh.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    struct PackageNode;

    class ModelNode
    {
    public:
        ModelNode(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds);
        ModelNode(const PackageNode& model);
        ~ModelNode();

        void Transform(const Mat4& transformMatrix);

		const std::string& GetName() const;

        const Mat4& GetTransformMatrix() const;
        const Vec3& GetAABBCenter() const;
        const Vec3& GetAABBExtent() const;

		std::vector<Mesh>& GetMeshes();
		const std::vector<Mesh>& GetMeshes() const;
        std::vector<ModelNode>& GetChildNodes();
		const std::vector<ModelNode>& GetChildNodes() const;


    private:
		const std::string mName;

        Mat4 mTransform;
        Vec3 mAABBCenter;
        Vec3 mAABBExtent;

		std::vector<Mesh> mMeshes;
        std::vector<ModelNode> mChildNodes;
    };
}