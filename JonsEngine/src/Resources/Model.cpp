#include "include/Resources/Model.h"

#include "include/Core/Math/Math.h"

namespace JonsEngine
{
    uint32_t CountNumMeshes(const PackageModel& model);
    uint32_t CountNumImmediateChildren(const PackageModel& model, const PackageNode& node);
    uint32_t CountNumChildren(const PackageModel& model, const PackageNode& node);
    void ReserveStorage(const PackageModel& model, ModelNode::NodeContainer& nodeContainer, ModelNode::MeshContainer& meshContainer);


    Model::Model(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID) : mName(name), mStaticAABB(minBounds, maxBounds)
    {
        const ModelNodeIndex rootNodeIndex = 0;

        mMeshes.emplace_back(name, meshID, INVALID_MATERIAL_ID, minBounds, maxBounds);
        mNodes.emplace_back(name, rootNodeIndex, minBounds, maxBounds, ModelNode::ImmediateChildrenIterator(mNodes.end(), mNodes.end()), ModelNode::AllChildrenIterator(mNodes.end(), mNodes.end()),
            ModelNode::MeshIterator(mMeshes.begin(), mMeshes.end()), mNodes.end());
    }

    Model::Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData) : mName(pkgModel.mName), mStaticAABB(pkgModel.mStaticAABB.mMinBounds, pkgModel.mStaticAABB.mMaxBounds)
    {
        ReserveStorage(pkgModel, mNodes, mMeshes);

        ParseNodes(pkgModel, initData, pkgModel.mNodes.front(), mNodes.end());
        
        for (const PackageAnimation& pkgAnimation : pkgModel.mAnimations)
            mAnimations.emplace_back(pkgAnimation, mNodes.size());
    }

    // node container needs special care due to reconstructing valid iterators
    Model::Model(const Model& otherModel) : mName(otherModel.mName), mStaticAABB(otherModel.mStaticAABB), mMeshes(otherModel.mMeshes), mAnimations(otherModel.mAnimations)
    {
        const uint32_t numNodes = otherModel.mNodes.size();
        mNodes.reserve(numNodes);

        for (const ModelNode& otherNode : otherModel.mNodes)
        {
            // rebuild mesh iter
            const auto meshBeginIter = otherNode.GetMeshes().begin();
            const auto meshEndIter = otherNode.GetMeshes().end();
            const uint32_t meshBeginIndex = meshBeginIter - otherModel.mMeshes.begin();
            const uint32_t numMeshes = meshEndIter - meshBeginIter;
            const auto meshIter = ModelNode::MeshIterator(mMeshes.begin() + meshBeginIndex, mMeshes.begin() + meshBeginIndex + numMeshes);

            // /all/immediate node iters
            // both iters use same begin/last, only advances differently
            const auto iterNodesBegin = otherModel.mNodes.begin();
            const uint32_t firstChildIndex = otherNode.GetAllChildren().begin() - iterNodesBegin;
            const uint32_t pastLastChildIndex = otherNode.GetAllChildren().end() - iterNodesBegin;
            const auto immChildrenIter = ModelNode::ImmediateChildrenIterator(mNodes.begin() + firstChildIndex, mNodes.begin() + pastLastChildIndex);
            const auto allChildrenIter = ModelNode::AllChildrenIterator(mNodes.begin() + firstChildIndex, mNodes.begin() + pastLastChildIndex);

            // nodes next iter
            auto immChildNext = otherNode.GetImmediateChildren().begin();
            auto immChildEnd = otherNode.GetImmediateChildren().end();
            uint32_t numChildren = std::distance(immChildNext, immChildEnd);
            uint32_t nextChildIndex = immChildEnd - iterNodesBegin;
            if (numChildren > 0)
            {
                ++immChildNext;
                const uint32_t nextChildIndex = immChildNext - iterNodesBegin;
            }

            const auto childNextIter = mNodes.begin() + nextChildIndex;

            mNodes.emplace_back(otherNode.GetName(), otherNode.GetModelNodeIndex(), otherNode.GetLocalAABB().Min(), otherNode.GetLocalAABB().Max(), immChildrenIter, allChildrenIter, meshIter, childNextIter);
        }
    }

    Model::~Model()
    {
    }


    Model::MeshIterator Model::GetMeshes() const
    {
        return MeshIterator(mMeshes.cbegin(), mMeshes.cend());
    }

    Model::NodeIterator Model::GetNodes() const
    {
        return NodeIterator(mNodes.cbegin(), mNodes.cend());
    }

    Model::ModelAnimationIterator Model::GetAnimations() const
    {
        return ModelAnimationIterator(mAnimations.cbegin(), mAnimations.cend());
    }


    ModelAnimationIndex Model::GetAnimationIndex(const std::string& name) const
    {
        ModelAnimationIndex ret = INVALID_ANIMATION_INDEX;

        const auto iter = std::find_if(mAnimations.cbegin(), mAnimations.cend(), [&name](const ModelAnimation& animation) { return animation.GetName() == name; });
        if (iter != mAnimations.cend())
            ret = iter - mAnimations.cbegin();

        return ret;
    }

    const ModelAnimation& Model::GetAnimation(const ModelAnimationIndex animationIndex) const
    {
        return mAnimations.at(animationIndex);
    }

    
    const ModelNode& Model::GetRootNode() const
    {
        return mNodes.front();
    }

    const std::string& Model::GetName() const
    {
        return mName;
    }

    const AABB& Model::GetStaticAABB() const
    {
        return mStaticAABB;
    }


    void Model::ParseNodes(const PackageModel& model, const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode, const ModelNode::NodeIterator& next)
    {
        const uint32_t numChildren = CountNumChildren(model, pkgNode);
        // not -1 since its not added yet
        const uint32_t thisNodeIndex = mNodes.size();
        const uint32_t firstChildIndex = thisNodeIndex + 1;

        const auto iterFirstChild = mNodes.begin() + firstChildIndex;
        const auto iterPastLastChild = mNodes.begin() + firstChildIndex + numChildren;

        const auto allChildIter = ModelNode::AllChildrenIterator(iterFirstChild, iterPastLastChild);
        const auto immChildIter = ModelNode::ImmediateChildrenIterator(iterFirstChild, iterPastLastChild);
        const auto meshesIter = ParseMeshes(model, initDataList, pkgNode);
        mNodes.emplace_back(pkgNode, immChildIter, allChildIter, meshesIter, next);

        const uint32_t numImmediateChildren = CountNumImmediateChildren(model, pkgNode);
        uint32_t childNum = 1;
        uint32_t nextChildSiblingOffset = firstChildIndex;
        for (const PackageNode& child : model.mNodes)
        {
            if (child.mParentNodeIndex != pkgNode.mNodeIndex)
                continue;

            nextChildSiblingOffset += CountNumChildren(model, child) + 1;

            const bool isLastSibling = (childNum == numImmediateChildren);
            const auto nextSibling = isLastSibling ? mNodes.end() : mNodes.begin() + nextChildSiblingOffset;
            ParseNodes(model, initDataList, child, nextSibling);

            ++childNum;
        }
    }

    ModelNode::MeshIterator Model::ParseMeshes(const PackageModel& model, const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode)
    {
        const uint32_t numMeshes = pkgNode.mMeshes.size();
        if (numMeshes == 0)
            return ModelNode::MeshIterator(mMeshes.end(), mMeshes.end());

        const uint32_t meshesSizeBegin = mMeshes.size() == 0 ? 0 : mMeshes.size() - 1;
        for (const PackageMesh::MeshIndex meshIndex : pkgNode.mMeshes)
        {
            const PackageMesh& mesh = model.mMeshes.at(meshIndex);

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


    uint32_t CountNumMeshes(const PackageModel& model)
    {
        uint32_t numMeshes = 0;

        for (const PackageNode& node : model.mNodes)
            numMeshes += node.mMeshes.size();

        return numMeshes;
    }

    uint32_t CountNumImmediateChildren(const PackageModel& model, const PackageNode& node)
    {
        uint32_t numNodes = 0;

        for (const PackageNode& iterNode : model.mNodes)
        {
            if (iterNode.mParentNodeIndex == node.mNodeIndex)
                ++numNodes;
        }

        return numNodes;
    }

    // inefficient, but not a big concern
    uint32_t CountNumChildren(const PackageModel& model, const PackageNode& node)
    {
        uint32_t numNodes = 0;

        for (const PackageNode& iterNode : model.mNodes)
        {
            if (iterNode.mParentNodeIndex == node.mNodeIndex)
            {
                ++numNodes;
                numNodes += CountNumChildren(model, iterNode);
            }
        }

        return numNodes;
    }

    void ReserveStorage(const PackageModel& model, ModelNode::NodeContainer& nodeContainer, ModelNode::MeshContainer& meshContainer)
    {
        // count meshes/nodes and reserve() space in containers before parsing them
        // otherwise iterators gets invalidated as parsing goes on and containers grow
        const uint32_t numMeshes = CountNumMeshes(model);
        const uint32_t numNodes = model.mNodes.size();

        meshContainer.reserve(numMeshes);
        nodeContainer.reserve(numNodes);
    }
}