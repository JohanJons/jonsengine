#pragma once

#include "include/Resources/Mesh.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Containers/RangedIterator.hpp"
#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

#include <vector>
#include <string>
#include <tuple>

namespace JonsEngine
{
    typedef std::tuple<const PackageMesh&, const DX11MeshID, const DX11MaterialID> DX11ResourceMeshTuple;
    typedef std::vector<DX11ResourceMeshTuple> DX11ResourceMeshTuples;

    class ModelNode
    {
    public:
        typedef std::vector<ModelNode> NodeContainer;
        typedef std::vector<Mesh> MeshContainer;

        ModelNode(const PackageNode& pkgNode, const DX11ResourceMeshTuples& meshResources, NodeContainer& nodes, MeshContainer& meshes);
        ModelNode(NodeContainer& nodes, MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const Mat4& initialTransform, const DX11MeshID meshID);
        ~ModelNode();


        const std::string mName;
        const AABB mLocalAABB;
        const Mat4 mLocalTransform;

        ConstRangedIterator<NodeContainer> mChildren;
        ConstRangedIterator<MeshContainer> mMeshes;
    };
}