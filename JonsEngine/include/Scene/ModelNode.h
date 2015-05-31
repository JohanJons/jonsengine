#pragma once

#include "include/Scene/Mesh.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"
#include "include/Core/Utils/NonCopyable.h"
#include "include/Core/Math/AABB.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/IDMapPointer.hpp"

#include <string>
#include <vector>

namespace JonsEngine
{
	class DX11Renderer;
    struct PackageNode;

	typedef const std::function<MaterialPtr(const std::string& assetName, const JonsPackagePtr jonsPkg)>& LoadMaterialFunc;

    class ModelNode : NonCopyable
    {
    public:
        ModelNode(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material, IDMap<Mat4>& transformStorage);
        ModelNode(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageNode& node, const Mat4& parentTransform, LoadMaterialFunc loadMaterialFunction, IDMap<Mat4>& transformStorage);
        ModelNode(ModelNode&& other) throw();
        ~ModelNode();

		std::vector<Mesh>& GetMeshes();
		std::vector<ModelNode>& GetChildNodes();

        const Mat4& GetLocalTransform() const;
        IDMap<Mat4>::ItemID GetLocalTransformID() const;


		const std::string mName;
        const AABB mLocalAABB;


    private:
        IDMapPointer<Mat4> mLocalTransform;

		std::vector<Mesh> mMeshes;
        std::vector<ModelNode> mChildNodes;
    };
}