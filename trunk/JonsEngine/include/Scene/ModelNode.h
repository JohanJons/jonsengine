#pragma once

#include "include/Scene/Mesh.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>

namespace JonsEngine
{
	class DX11Renderer;
    struct PackageNode;

	typedef const std::function<MaterialPtr(const std::string& assetName, const JonsPackagePtr jonsPkg)>& LoadMaterialFunc;

    class ModelNode
    {
    public:
        ModelNode(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material);
        ModelNode(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageNode& node, LoadMaterialFunc loadMaterialFunction);
        ~ModelNode();

        const Mat4& GetTransformMatrix() const;

		const std::vector<Mesh>& GetMeshes() const;
		const std::vector<ModelNode>& GetChildNodes() const;


		const std::string mName;

        const Vec3 mAABBCenter;
        const Vec3 mAABBExtent;


    private:
        Mat4 mTransform;

		std::vector<Mesh> mMeshes;
        std::vector<ModelNode> mChildNodes;
    };
}