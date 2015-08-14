#include "include/Resources/Model.h"

namespace JonsEngine
{
    uint32_t CountNumMeshes(const PackageNode& node);
    uint32_t CountNumChildren(const PackageNode& node);


    Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID) : mName(name)
    {
        mMeshes.emplace_back(name, meshID, INVALID_MATERIAL_ID, minBounds, maxBounds);
        mNodes.emplace_back(name, minBounds, maxBounds, initialTransform, ModelNode::ImmediateChildrenIterator(mNodes.end(), mNodes.end()), ModelNode::AllChildrenIterator(mNodes.end(), mNodes.end()), ModelNode::MeshIterator(mMeshes.begin(), mMeshes.end()));
    }

    Model::Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData) : mName(pkgModel.mName)
    {
        // count meshes/nodes and reserve() space in containers before parsing them
        // otherwise iterators gets invalidated as parsing goes on
        const uint32_t numMeshes = CountNumMeshes(pkgModel.mRootNode);
        const uint32_t numNodes = CountNumChildren(pkgModel.mRootNode);
        mMeshes.reserve(numMeshes);
        mNodes.reserve(numNodes);

        ParseNodes(initData, pkgModel.mRootNode);
    }

    Model::~Model()
    {
    }


    const ModelNode& Model::GetRootNode() const
    {
        return mNodes.front();
    }


    void Model::ParseNodes(const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode)
    {
        const uint32_t meshesSizeBegin = mMeshes.size() == 0 ? 0 : mMeshes.size() - 1;
        for (const PackageMesh& mesh : pkgNode.mMeshes)
        {
            auto iter = std::find_if(initDataList.cbegin(), initDataList.cend(), [mesh](const ModelNode::InitData& data) { return mesh.mName.compare(std::get<0>(data).mName) == 0; });
            assert(iter != initDataList.cend());

            const ModelNode::InitData& initData = *iter;
            const DX11MeshID meshID = std::get<1>(initData);
            const MaterialID defaultMaterialID = std::get<2>(initData);

            mMeshes.emplace_back(mesh.mName, meshID, defaultMaterialID, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
        }
        const uint32_t meshesSizeEnd = mMeshes.size();

        const uint32_t numChildren = CountNumChildren(pkgNode) - 1;
        const uint32_t nodesSizeBegin = mNodes.size() + 1;
        const auto allChildIter = ModelNode::AllChildrenIterator(mNodes.begin() + nodesSizeBegin, mNodes.begin() + nodesSizeBegin + numChildren);

        const auto immChildIter = ModelNode::ImmediateChildrenIterator(mNodes.begin() + nodesSizeBegin, mNodes.begin() + nodesSizeBegin + numChildren);

        const auto meshesIter = ModelNode::MeshIterator(mMeshes.begin() + meshesSizeBegin, mMeshes.begin() + meshesSizeEnd);

        mNodes.emplace_back(pkgNode, immChildIter, allChildIter, meshesIter);
        // dont count oneself
        /*const uint32_t numChildren = CountNumNodes(pkgNode) - 1;
        // first child is one pos ahead
        const uint32_t nodesSizeBegin = mNodes.size() + 1;

        mNodes.emplace_back(pkgNode, ModelNode::AllChildrenIterator(mNodes.begin() + nodesSizeBegin, mNodes.begin() + nodesSizeBegin + numChildren), ModelNode::MeshIterator(mMeshes.begin() + meshesSizeBegin, mMeshes.begin() + meshesSizeEnd));

        ModelNode* prevNode = nullptr;
        uint32_t childNum = 0;
        const uint32_t numNodesToFirstChild = mNodes.size();
        for (const PackageNode& child : pkgNode.mChildNodes)
        {
            ParseNodes(initDataList, child);

            ModelNode* newNode = &mNodes.at(numNodesToFirstChild + childNum);
            if (prevNode)
                prevNode->mNext = newNode;
            prevNode = newNode;

            // jumps to the next child of pkgNode which position depends on current childs num children
            childNum += CountNumNodes(child);
        }*/
    }


    uint32_t CountNumMeshes(const PackageNode& node)
    {
        uint32_t numMeshes = 0;

        for (const PackageMesh& mesh : node.mMeshes)
            ++numMeshes;

        for (const PackageNode& child : node.mChildNodes)
            numMeshes += CountNumMeshes(child);

        return numMeshes;
    }

    uint32_t CountNumChildren(const PackageNode& node)
    {
        uint32_t numNodes = 0;

        for (const PackageNode& child : node.mChildNodes)
        {
            ++numNodes;
            numNodes += CountNumChildren(child);
        }

        return numNodes;
    }
}