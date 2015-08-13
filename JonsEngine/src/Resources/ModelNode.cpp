#include "include/Resources/ModelNode.h"

#include <algorithm>

namespace JonsEngine
{
    typedef ModelNode::InitDataList InitDataList;
    typedef ModelNode::NodeContainer NodeContainer;
    typedef ModelNode::MeshContainer MeshContainer;

    ModelNode::MeshIterator ParseMeshes(MeshContainer& meshes, const InitDataList& initData, const PackageNode& pkgNode);
    ModelNode::MeshIterator ParseMesh(MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID);
    template <typename Container>
    size_t GetStartIndex(const Container& nodes);



    //
    // ModelNode::ImmediateChildrenIter
    //

    ModelNode::ImmediateChildrenIter::ImmediateChildrenIter(const NodeIterator& iter) : NodeIterator(iter)
    {
    }

    ModelNode::ImmediateChildrenIter& ModelNode::ImmediateChildrenIter::operator++()
    {
        this->_Ptr = this->_Ptr->mNext;

        assert(this->_Ptr != nullptr);

        return *this;
    }



    //
    // ModelNode
    //

    ModelNode::ModelNode(const PackageNode& pkgNode, const InitDataList& initData, NodeContainer& nodes, MeshContainer& meshes) :
        mName(pkgNode.mName), mLocalAABB(pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds), mLocalTransform(pkgNode.mTransform), mImmediateChildNodes(ParseChildNodes<ModelNode::ImmediateChildrenIterator>(pkgNode, initData, nodes, meshes)),
        mAllChildNodes(ParseChildNodes<ModelNode::AllChildrenIterator>(pkgNode, initData, nodes, meshes)), mMeshes(ParseMeshes(meshes, initData, pkgNode))
    {
    }

    ModelNode::ModelNode(NodeContainer& nodes, MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const Mat4& initialTransform, const DX11MeshID meshID) :
        mName(name), mLocalAABB(minBounds, maxBounds), mLocalTransform(initialTransform), mImmediateChildNodes(ImmediateChildrenIter(nodes.end()), ImmediateChildrenIter(nodes.end())),
        mAllChildNodes(ImmediateChildrenIter(nodes.end()), ImmediateChildrenIter(nodes.end())), mMeshes(ParseMesh(meshes, name, minBounds, maxBounds, meshID))
    {
    }

    ModelNode::~ModelNode()
    {
    }



    //
    // Initialization functions
    //
    
    template <typename ChildIterator>
    ChildIterator ModelNode::ParseChildNodes(const PackageNode& pkgNode, const InitDataList& initData, NodeContainer& nodes, MeshContainer& meshes)
    {
        const size_t indexBegin = GetStartIndex<NodeContainer>(nodes);

        ModelNode* prevNode = nullptr;
        for (const PackageNode& node : pkgNode.mChildNodes)
        {
            nodes.emplace_back(node, initData, nodes, meshes);
            ModelNode* newNode = &nodes.back();

            if (prevNode)
                prevNode->mNext = newNode;
            prevNode = newNode;
        }
        // TODO
        ...
        //prevNode->mNext = mNext;

        const size_t indexEnd = nodes.size();

        assert((indexEnd - indexBegin) > 0);

        return ChildIterator(ChildIterator::Iterator(nodes.begin() + indexBegin), ChildIterator::Iterator(nodes.end()));
    }

    ModelNode::MeshIterator ParseMeshes(MeshContainer& meshes, const InitDataList& initDataList, const PackageNode& pkgNode)
    {
        const size_t sizeBegin = GetStartIndex<MeshContainer>(meshes);

        for (const PackageMesh& mesh : pkgNode.mMeshes)
        {
            auto iter = std::find_if(initDataList.cbegin(), initDataList.cend(), [mesh](const ModelNode::InitData& data) { return mesh.mName.compare(std::get<0>(data).mName); });
            assert(iter != initDataList.cend());

            const ModelNode::InitData& initData = *iter;
            const DX11MeshID meshID = std::get<1>(initData);
            const MaterialID defaultMaterialID = std::get<2>(initData);

            meshes.emplace_back(mesh.mName, meshID, defaultMaterialID, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
        }

        const size_t sizeEnd = meshes.size();

        assert((sizeEnd - sizeBegin) > 0);

        return ModelNode::MeshIterator(meshes, sizeBegin, sizeEnd);
    }

    ModelNode::MeshIterator ParseMesh(MeshContainer& meshes, const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID)
    {
        const size_t sizeBegin = GetStartIndex<MeshContainer>(meshes);

        meshes.emplace_back(name, meshID, INVALID_DX11_MATERIAL_ID, minBounds, maxBounds);

        const size_t sizeEnd = meshes.size();

        return ModelNode::MeshIterator(meshes, sizeBegin, sizeEnd);
    }

    template <typename Container>
    size_t GetStartIndex(const Container& container)
    {
        return container.size() == 0 ? 0 : container.size() - 1;
    }
}