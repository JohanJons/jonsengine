#include "include/Resources/Model.h"

namespace JonsEngine
{
    uint32_t CountNumMeshes(const PackageNode& node);
    uint32_t CountNumChildren(const PackageNode& node);
    void ReserveStorage(const PackageModel& model, ModelNode::NodeContainer& nodeContainer, ModelNode::MeshContainer& meshContainer);


    Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID) : mName(name)
    {
        mMeshes.emplace_back(name, meshID, INVALID_MATERIAL_ID, minBounds, maxBounds);
        mNodes.emplace_back(name, minBounds, maxBounds, initialTransform, ModelNode::ImmediateChildrenIterator(mNodes.end(), mNodes.end()), ModelNode::AllChildrenIterator(mNodes.end(), mNodes.end()),
            ModelNode::MeshIterator(mMeshes.begin(), mMeshes.end()), mNodes.end());
    }

    Model::Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData) : mName(pkgModel.mName)
    {
        ReserveStorage(pkgModel, mNodes, mMeshes);

        ParseNodes(initData, pkgModel.mRootNode, mNodes.end());
    }

    // node container needs special care due to reconstructing valid iterators
    Model::Model(const Model& otherModel) : mName(otherModel.mName), mMeshes(otherModel.mMeshes)
    {
        const uint32_t numNodes = otherModel.mNodes.size();
        mNodes.reserve(numNodes);

        for (const ModelNode& otherNode : otherModel.mNodes)
        {
            // rebuild mesh iter
            const auto meshBeginIter = otherNode.mMeshes.begin();
            const auto meshEndIter = otherNode.mMeshes.end();
            const uint32_t meshBeginIndex = meshBeginIter - otherModel.mMeshes.begin();
            const uint32_t numMeshes = meshEndIter - meshBeginIter;
            const auto meshIter = ModelNode::MeshIterator(mMeshes.begin() + meshBeginIndex, mMeshes.begin() + meshBeginIndex + numMeshes);

            // /all/immediate node iters
            // both iters use same begin/last, only advances differently
            const auto iterNodesBegin = otherModel.mNodes.begin();
            const uint32_t firstChildIndex = otherNode.mAllChildNodes.begin() - iterNodesBegin;
            const uint32_t pastLastChildIndex = otherNode.mAllChildNodes.end() - iterNodesBegin;
            const auto immChildrenIter = ModelNode::ImmediateChildrenIterator(mNodes.begin() + firstChildIndex, mNodes.begin() + pastLastChildIndex);
            const auto allChildrenIter = ModelNode::AllChildrenIterator(mNodes.begin() + firstChildIndex, mNodes.begin() + pastLastChildIndex);

            // all node iter
            //const uint32_t allChildFirstIndex = otherNode.mAllChildNodes.begin() - otherNodesBegin;
            //const uint32_t allChildEndIndex = otherNode.mAllChildNodes.end() - otherNodesBegin;
            //const auto allChildrenIter = ModelNode::AllChildrenIterator(mNodes.begin() + allChildFirstIndex, mNodes.begin() + allChildEndIndex);

            // nodes next iter
            auto immChildNext = otherNode.mImmediateChildNodes.begin();
            auto immChildEnd = otherNode.mImmediateChildNodes.end();
            uint32_t numChildren = std::distance(immChildNext, immChildEnd);
            uint32_t nextChildIndex = immChildEnd - iterNodesBegin;
            if (numChildren > 0)
            {
                ++immChildNext;
                const uint32_t nextChildIndex = immChildNext - iterNodesBegin;
            }

            //++immChildNext;
            //const uint32_t nextChildIndex = immChildNext - iterNodesBegin;
            const auto childNextIter = mNodes.begin() + nextChildIndex;

            mNodes.emplace_back(otherNode.mName, otherNode.mLocalAABB.Min(), otherNode.mLocalAABB.Max(), otherNode.mLocalTransform, immChildrenIter, allChildrenIter, meshIter, childNextIter);
        }
    }

    Model::~Model()
    {
    }


    const ModelNode& Model::GetRootNode() const
    {
        return mNodes.front();
    }


    void Model::ParseNodes(const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode, const ModelNode::NodeIterator& next)
    {
        const uint32_t numChildren = CountNumChildren(pkgNode);
        // not -1 since its not added yet
        const uint32_t thisNodeIndex = mNodes.size();
        const uint32_t firstChildIndex = thisNodeIndex + 1;
        //const uint32_t siblingIndex = thisNodeIndex + numChildren + 1;

        const auto iterFirstChild = mNodes.begin() + firstChildIndex;
        const auto iterPastLastChild = mNodes.begin() + firstChildIndex + numChildren;

        const auto allChildIter = ModelNode::AllChildrenIterator(iterFirstChild, iterPastLastChild);
        const auto immChildIter = ModelNode::ImmediateChildrenIterator(iterFirstChild, iterPastLastChild);
        const auto meshesIter = ParseMeshes(initDataList, pkgNode);
        //ModelNode* nextSibling = &mNodes[siblingIndex];//nullptr;// GetNextSibling(node, parent);
        mNodes.emplace_back(pkgNode, immChildIter, allChildIter, meshesIter, next);

        const uint32_t numImmediateChildren = pkgNode.mChildNodes.size();
        uint32_t childNum = 1;
        uint32_t nextChildSiblingOffset = firstChildIndex;
        for (const PackageNode& child : pkgNode.mChildNodes)
        {
            // calculcate offset to next sibling
            nextChildSiblingOffset += CountNumChildren(child) + 1;

            const bool isLastSibling = (childNum == numImmediateChildren);
            const auto nextSibling = isLastSibling ? mNodes.end() : mNodes.begin() + nextChildSiblingOffset;
            ParseNodes(initDataList, child, nextSibling);

            ++childNum;
        }
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

    ModelNode::MeshIterator Model::ParseMeshes(const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode)
    {
        const uint32_t numMeshes = pkgNode.mMeshes.size();
        if (numMeshes == 0)
            return ModelNode::MeshIterator(mMeshes.end(), mMeshes.end());

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

        return ModelNode::MeshIterator(mMeshes.begin() + meshesSizeBegin, mMeshes.begin() + meshesSizeEnd);
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

    void ReserveStorage(const PackageModel& model, ModelNode::NodeContainer& nodeContainer, ModelNode::MeshContainer& meshContainer)
    {
        // count meshes/nodes and reserve() space in containers before parsing them
        // otherwise iterators gets invalidated as parsing goes on and containers grow
        const uint32_t numMeshes = CountNumMeshes(model.mRootNode);
        // + 1 to include root
        const uint32_t numNodes = CountNumChildren(model.mRootNode) + 1;

        meshContainer.reserve(numMeshes);
        nodeContainer.reserve(numNodes);
    }
}