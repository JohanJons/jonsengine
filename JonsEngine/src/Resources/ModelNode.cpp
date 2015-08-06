#include "include/Resources/ModelNode.h"

#include <algorithm>

namespace JonsEngine
{
    typedef ModelNode::NodeContainer NodeContainer;
    typedef ModelNode::MeshContainer MeshContainer;

    ConstRangedIterator<NodeContainer> ParseChildren(NodeContainer& nodes, MeshContainer& meshes, const PackageNode& pkgNode);
    ConstRangedIterator<MeshContainer> ParseMeshes(MeshContainer& meshes, const DX11ResourceMeshTuples& meshResources, const PackageNode& pkgNode);
    ConstRangedIterator<MeshContainer> ParseMeshes(MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID);


    ModelNode::ModelNode(const PackageNode& pkgNode, const DX11ResourceMeshTuples& meshResources, NodeContainer& nodes, MeshContainer& meshes) :
        mName(pkgNode.mName), mLocalAABB(pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds), mLocalTransform(pkgNode.mTransform), mChildren(ParseChildren(nodes, meshes, pkgNode)), mMeshes(ParseMeshes(meshes, meshResources, pkgNode))
    {
    }

    ModelNode::ModelNode(NodeContainer& nodes, MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const Mat4& initialTransform, const DX11MeshID meshID) :
        mName(name), mLocalAABB(minBounds, maxBounds), mLocalTransform(initialTransform), mChildren(nodes, nodes.size() - 1, nodes.size() - 1),
        mMeshes(ParseMeshes(meshes, name, minBounds, maxBounds, meshID))
    {
    }

    ModelNode::~ModelNode()
    {
    }


    ConstRangedIterator<NodeContainer> ParseChildren(NodeContainer& nodes, const DX11ResourceMeshTuples& meshResources, MeshContainer& meshes, const PackageNode& pkgNode)
    {
        const size_t sizeBegin = nodes.size() == 0 ? 0 : nodes.size() - 1;

        for (const PackageNode& node : pkgNode.mChildNodes)
        {
            nodes.emplace_back(node, meshResources, nodes, meshes);
        }

        const size_t sizeEnd = nodes.size() - 1;

        return ConstRangedIterator<NodeContainer>(nodes, sizeBegin, sizeEnd);
    }

    ConstRangedIterator<MeshContainer> ParseMeshes(MeshContainer& meshes, const DX11ResourceMeshTuples& meshResources, const PackageNode& pkgNode)
    {
        const size_t sizeBegin = meshes.size() == 0 ? 0 : meshes.size() - 1;
        
        for (const PackageMesh& mesh : pkgNode.mMeshes)
        {
            auto iter = std::find_if(meshResources.cbegin(), meshResources.cend(), [mesh](const DX11ResourceMeshTuple& pair) { return mesh.mName.compare(std::get<0>(pair).mName); });
            assert(iter != meshResources.cend());

            const DX11ResourceMeshTuple& meshResource = *iter;
            const DX11MeshID meshID = std::get<1>(meshResource);
            const DX11MaterialID materialID = std::get<2>(meshResource);

            meshes.emplace_back(mesh.mName, meshID, materialID, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
        }

        const size_t sizeEnd = meshes.size() - 1;

        return ConstRangedIterator<MeshContainer>(meshes, sizeBegin, sizeEnd);
    }

    ConstRangedIterator<MeshContainer> ParseMeshes(MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID)
    {
        const size_t sizeBegin = meshes.size() == 0 ? 0 : meshes.size() - 1;

        meshes.emplace_back(name, meshID, INVALID_DX11_MATERIAL_ID, minBounds, maxBounds);

        const size_t sizeEnd = meshes.size() - 1;

        return ConstRangedIterator<MeshContainer>(meshes, sizeBegin, sizeEnd);
    }
}