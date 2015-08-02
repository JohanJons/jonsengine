#include "include/Resources/ModelNode.h"

namespace JonsEngine
{
    ConstRangedIterator<ModelNode::NodeContainer> ParseChildren(ModelNode::NodeContainer& nodes, const PackageNode& pkgNode);
    ConstRangedIterator<ModelNode::MeshContainer> ParseMeshes(ModelNode::MeshContainer& meshes, const PackageNode& pkgNode);
    ConstRangedIterator<ModelNode::MeshContainer> ParseMeshes(ModelNode::MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID);


    ModelNode::ModelNode(const PackageNode& pkgNode, NodeContainer& nodes, MeshContainer& meshes) :
        mName(pkgNode.mName), mLocalAABB(pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds), mLocalTransform(pkgNode.mTransform), mChildren(ParseChildren(nodes, pkgNode)), mMeshes(ParseMeshes(meshes, pkgNode))
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


    ConstRangedIterator<ModelNode::NodeContainer> ParseChildren(ModelNode::NodeContainer& nodes, const PackageNode& pkgNode)
    {
        const size_t sizeBegin = nodes.size() == 0 ? 0 : nodes.size() - 1;

        for (const PackageNode& node : pkgNode.mChildNodes)
        {

        }

        const size_t sizeEnd = nodes.size() - 1;

        return ConstRangedIterator<ModelNode::NodeContainer>(nodes, sizeBegin, sizeEnd);
    }

    ConstRangedIterator<ModelNode::MeshContainer> ParseMeshes(ModelNode::MeshContainer& meshes, const PackageNode& pkgNode)
    {
        const size_t sizeBegin = meshes.size() == 0 ? 0 : meshes.size() - 1;
        
        for (const PackageMesh& mesh : pkgNode.mMeshes)
        {
            // ...
        }

        const size_t sizeEnd = meshes.size() - 1;

        return ConstRangedIterator<ModelNode::MeshContainer>(meshes, sizeBegin, sizeEnd);
    }

    ConstRangedIterator<ModelNode::MeshContainer> ParseMeshes(ModelNode::MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID)
    {
        const size_t sizeBegin = meshes.size() == 0 ? 0 : meshes.size() - 1;
        meshes.emplace_back(name, meshID, minBounds, maxBounds);
        const size_t sizeEnd = meshes.size() - 1;

        return ConstRangedIterator<ModelNode::MeshContainer>(meshes, sizeBegin, sizeEnd);
    }
}