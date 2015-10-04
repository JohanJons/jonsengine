#include "include/Resources/Model.h"

#include "include/Core/Math/Math.h"

namespace JonsEngine
{
    uint32_t CountNumMeshes(const PackageNode& node);
    uint32_t CountNumChildren(const PackageNode& node);
    void ReserveStorage(const PackageModel& model, ModelNode::NodeContainer& nodeContainer, ModelNode::MeshContainer& meshContainer);


    Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID) : mName(name)
    {
        const PackageNode::PackageNodeID onlyNodeID = 1;

        mMeshes.emplace_back(name, meshID, INVALID_MATERIAL_ID, minBounds, maxBounds);
        mNodes.emplace_back(name, onlyNodeID, minBounds, maxBounds, initialTransform, ModelNode::ImmediateChildrenIterator(mNodes.end(), mNodes.end()), ModelNode::AllChildrenIterator(mNodes.end(), mNodes.end()),
            ModelNode::MeshIterator(mMeshes.begin(), mMeshes.end()), mNodes.end());
    }

    Model::Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData) : mName(pkgModel.mName)
    {
        ReserveStorage(pkgModel, mNodes, mMeshes);

        ParseNodes(initData, gIdentityMatrix, pkgModel.mRootNode, mNodes.end());
        
        for (const PackageAnimation& pkgAnimation : pkgModel.mAnimations)
            mAnimations.emplace_back(pkgAnimation);
    }

    // node container needs special care due to reconstructing valid iterators
    Model::Model(const Model& otherModel) : mName(otherModel.mName), mMeshes(otherModel.mMeshes), mAnimations(otherModel.mAnimations)
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

            mNodes.emplace_back(otherNode.GetName(), otherNode.GetModelNodeID(), otherNode.GetLocalAABB().Min(), otherNode.GetLocalAABB().Max(), otherNode.GetLocalTransform(), immChildrenIter, allChildrenIter, meshIter, childNextIter);
        }
    }

    Model::~Model()
    {
    }


    Model::AnimationIterator Model::GetAnimations() const
    {
        return mAnimations.cbegin();
    }

    AnimationID Model::GetAnimation(const std::string& name) const
    {
        AnimationID ret = INVALID_ANIMATION_ID;

        const auto iter = std::find_if(mAnimations.cbegin(), mAnimations.cend(), [&name](const ModelAnimation& animation) { return animation.GetName() == name; });
        if (iter != mAnimations.end())
            ret = iter->GetAnimationID();

        return ret;
    }

    const ModelNode& Model::GetRootNode() const
    {
        return mNodes.front();
    }

    const std::string& Model::GetName() const
    {
        return mName;
    }


    void Model::ParseNodes(const ModelNode::InitDataList& initDataList, const Mat4& parentTransform, const PackageNode& pkgNode, const ModelNode::NodeIterator& next)
    {
        const uint32_t numChildren = CountNumChildren(pkgNode);
        // not -1 since its not added yet
        const uint32_t thisNodeIndex = mNodes.size();
        const uint32_t firstChildIndex = thisNodeIndex + 1;

        const auto iterFirstChild = mNodes.begin() + firstChildIndex;
        const auto iterPastLastChild = mNodes.begin() + firstChildIndex + numChildren;

        const auto allChildIter = ModelNode::AllChildrenIterator(iterFirstChild, iterPastLastChild);
        const auto immChildIter = ModelNode::ImmediateChildrenIterator(iterFirstChild, iterPastLastChild);
        const auto meshesIter = ParseMeshes(initDataList, pkgNode);
        mNodes.emplace_back(pkgNode, parentTransform, immChildIter, allChildIter, meshesIter, next);

        const Mat4& localTransform = mNodes.back().GetLocalTransform();
        const uint32_t numImmediateChildren = pkgNode.mChildNodes.size();
        uint32_t childNum = 1;
        uint32_t nextChildSiblingOffset = firstChildIndex;
        for (const PackageNode& child : pkgNode.mChildNodes)
        {
            // calculcate offset to next sibling
            nextChildSiblingOffset += CountNumChildren(child) + 1;

            const bool isLastSibling = (childNum == numImmediateChildren);
            const auto nextSibling = isLastSibling ? mNodes.end() : mNodes.begin() + nextChildSiblingOffset;
            ParseNodes(initDataList, localTransform, child, nextSibling);

            ++childNum;
        }
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