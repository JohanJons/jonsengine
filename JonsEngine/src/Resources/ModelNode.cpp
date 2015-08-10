#include "include/Resources/ModelNode.h"

#include <algorithm>

namespace JonsEngine
{
    typedef ModelNode::InitData InitData;
    typedef ModelNode::InitDataList InitDataList;

    typedef ModelNode::NodeContainer NodeContainer;
    typedef ModelNode::MeshContainer MeshContainer;

    typedef ModelNode::MeshIterator MeshIterator;
    //typedef ModelNode::NodeIterator NodeIterator;

    NodeIterator ParseChildren(NodeContainer& nodes, MeshContainer& meshes, const PackageNode& pkgNode);
    MeshIterator ParseMeshes(MeshContainer& meshes, const InitDataList& meshResources, const PackageNode& pkgNode);
    MeshIterator ParseMeshes(MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID);


    ModelNode::ModelNode(const PackageNode& pkgNode, const InitDataList& initData, NodeContainer& nodes, MeshContainer& meshes) :
        mName(pkgNode.mName), mLocalAABB(pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds), mLocalTransform(pkgNode.mTransform), mChildren(ParseChildren(nodes, meshes, pkgNode)), mMeshes(ParseMeshes(meshes, initData, pkgNode))
    {
    }

    ModelNode::ModelNode(NodeContainer& nodes, MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const Mat4& initialTransform, const DX11MeshID meshID) :
        mName(name), mLocalAABB(minBounds, maxBounds), mLocalTransform(initialTransform), mChildren(nodes, nodes.size() - 1, nodes.size() - 1), mMeshes(ParseMeshes(meshes, name, minBounds, maxBounds, meshID))
    {
    }

    ModelNode::~ModelNode()
    {
    }


    NodeIterator ParseChildren(NodeContainer& nodes, const InitDataList& initDataList, MeshContainer& meshes, const PackageNode& pkgNode)
    {
        const size_t sizeBegin = nodes.size() == 0 ? 0 : nodes.size() - 1;

        for (const PackageNode& node : pkgNode.mChildNodes)
        {
            nodes.emplace_back(node, initDataList, nodes, meshes);
        }

        const size_t sizeEnd = nodes.size() - 1;

        assert((sizeEnd - sizeBegin) > 0);

        return NodeIterator(nodes, sizeBegin, sizeEnd);
    }

    MeshIterator ParseMeshes(MeshContainer& meshes, const InitDataList& initDataList, const PackageNode& pkgNode)
    {
        const size_t sizeBegin = meshes.size() == 0 ? 0 : meshes.size() - 1;
        
        for (const PackageMesh& mesh : pkgNode.mMeshes)
        {
            auto iter = std::find_if(initDataList.cbegin(), initDataList.cend(), [mesh](const InitData& data) { return mesh.mName.compare(std::get<0>(data).mName); });
            assert(iter != initDataList.cend());

            const InitData& initData = *iter;
            const DX11MeshID meshID = std::get<1>(initData);
            const MaterialID defaultMaterialID = std::get<2>(initData);

            meshes.emplace_back(mesh.mName, meshID, defaultMaterialID, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
        }

        const size_t sizeEnd = meshes.size() - 1;

        assert((sizeEnd - sizeBegin) > 0);

        return MeshIterator(meshes, sizeBegin, sizeEnd);
    }

    MeshIterator ParseMeshes(MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID)
    {
        const size_t sizeBegin = meshes.size() == 0 ? 0 : meshes.size() - 1;

        meshes.emplace_back(name, meshID, INVALID_DX11_MATERIAL_ID, minBounds, maxBounds);

        const size_t sizeEnd = meshes.size() - 1;

        return MeshIterator(meshes, sizeBegin, sizeEnd);
    }
}