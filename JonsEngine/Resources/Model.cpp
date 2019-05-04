#include "Resources/Model.h"

#include "Core/Math/MathUtils.h"
#include "Resources/JonsPackage.h"

namespace JonsEngine
{
    uint32_t CountNumMeshes(const PackageModel& model);
    uint32_t CountNumImmediateChildren(const PackageModel& model, const PackageNode& node);
    uint32_t CountNumChildren(const PackageModel& model, const PackageNode& node);
    void ReserveStorage(const PackageModel& model, ModelNode::NodeContainer& nodeContainer, ModelNode::MeshContainer& meshContainer, Model::AnimationMap& animationMap, Model::AnimationNameMap& animationNameMap);


    Model::Model(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID) :
        mName(name),
        mStaticAABB(minBounds, maxBounds)
    {
        const ModelNodeIndex rootNodeIndex = INVALID_MODEL_NODE_INDEX;

        mMeshes.emplace_back(name, meshID, INVALID_MATERIAL_ID, minBounds, maxBounds);
        mNodes.emplace_back(name, rootNodeIndex, gIdentityMatrix, ModelNode::ImmediateChildrenIterator(mNodes.end(), mNodes.end()),
			ModelNode::AllChildrenIterator(mNodes.end(), mNodes.end()), ModelNode::MeshIterator(mMeshes.begin(), mMeshes.end()), mNodes.end());
    }

    Model::Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData) :
        mName(pkgModel.mName),
        mStaticAABB(pkgModel.mStaticAABB.mMinBounds, pkgModel.mStaticAABB.mMaxBounds),
		mParentMap(pkgModel.mBoneParentMap)
    {
        ReserveStorage(pkgModel, mNodes, mMeshes, mAnimations, mAnimationNameMap);

        ParseNodes(pkgModel, initData, pkgModel.mNodes.front(), mNodes.end());
		ParseSkeleton(pkgModel);
		ParseAnimations(pkgModel);
    }

    // node container needs special care due to reconstructing valid iterators
	// same for animation container due to new references
    Model::Model(const Model& otherModel) :
        mName(otherModel.mName),
        mStaticAABB(otherModel.mStaticAABB),
        mAnimationNameMap(otherModel.mAnimationNameMap),
        mMeshes(otherModel.mMeshes),
		mParentMap(otherModel.mParentMap),
		mBoneOffsetTransforms(otherModel.mBoneOffsetTransforms),
		mAnimationIDGen(otherModel.mAnimationIDGen)
    {
		CopyNodeHierarchy(otherModel);
		CopyAnimationHierarchy(otherModel);
    }


	bool Model::HasAnimations() const
	{
		return !mAnimations.empty();
	}


    Model::MeshIterator Model::GetMeshes() const
    {
        return MeshIterator(mMeshes.cbegin(), mMeshes.cend());
    }

    Model::NodeIterator Model::GetNodes() const
    {
        return NodeIterator(mNodes.cbegin(), mNodes.cend());
    }

    Model::AnimationIterator Model::GetAnimations() const
    {
        return AnimationIterator(mAnimations.cbegin(), mAnimations.cend());
    }


    AnimationID Model::GetAnimationID(const std::string& name) const
    {
		const AnimationID id = mAnimationNameMap.at(name);
		assert(id != INVALID_ANIMATION_ID);

        return id;
    }

	const Animation& Model::GetAnimation(const AnimationID id) const
	{
		assert(id != INVALID_ANIMATION_ID);

		return mAnimations.at(id);
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
        const uint32_t thisNodeIndex = static_cast<uint32_t>( mNodes.size() );
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

	void Model::ParseSkeleton(const PackageModel& model)
	{
		for (const auto& pkgBone : model.mSkeleton)
			mBoneOffsetTransforms.emplace_back(pkgBone.mOffsetMatrix);
	}

    ModelNode::MeshIterator Model::ParseMeshes(const PackageModel& model, const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode)
    {
        const uint32_t numMeshes = static_cast<uint32_t>( pkgNode.mMeshes.size() );
        if (numMeshes == 0)
            return ModelNode::MeshIterator(mMeshes.end(), mMeshes.end());

        const uint32_t meshesSizeBegin = mMeshes.empty() ? 0 : static_cast<uint32_t>( mMeshes.size() ) - 1;
        for (const PackageMesh::MeshIndex meshIndex : pkgNode.mMeshes)
        {
            const PackageMesh& mesh = model.mMeshes.at(meshIndex);

            auto iter = std::find_if(initDataList.cbegin(), initDataList.cend(), [&mesh](const ModelNode::InitData& data) { return mesh.mName.compare(std::get<0>(data).mName) == 0; });
            assert(iter != initDataList.cend());

            const ModelNode::InitData& initData = *iter;
            const DX11MeshID meshID = std::get<1>(initData);
            const MaterialID defaultMaterialID = std::get<2>(initData);

            mMeshes.emplace_back(mesh.mName, meshID, defaultMaterialID, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
        }
        const uint32_t meshesSizeEnd = static_cast<uint32_t>( mMeshes.size() );

        return ModelNode::MeshIterator(mMeshes.begin() + meshesSizeBegin, mMeshes.begin() + meshesSizeEnd);
    }

	void Model::ParseAnimations(const PackageModel& pkgModel)
	{
		for (const PackageAnimation& pkgAnim : pkgModel.mAnimations)
		{
			const AnimationID nextAnimID = mAnimationIDGen.GenerateID();
			const auto animationDuration = Milliseconds(pkgAnim.mDurationInMilliseconds);

			mAnimations.emplace(std::piecewise_construct, std::forward_as_tuple(nextAnimID), std::forward_as_tuple(pkgAnim, mParentMap, mBoneOffsetTransforms));
			mAnimationNameMap.emplace(std::piecewise_construct, std::forward_as_tuple(pkgAnim.mName), std::forward_as_tuple(nextAnimID));
		}
	}


	void Model::CopyNodeHierarchy(const Model& otherModel)
	{
		const uint32_t numNodes = static_cast<uint32_t>( otherModel.mNodes.size() );
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

			mNodes.emplace_back(otherNode.GetName(), otherNode.GetModelNodeIndex(), otherNode.GetLocalTransform(), immChildrenIter, allChildrenIter, meshIter, childNextIter);
		}
	}

	void Model::CopyAnimationHierarchy(const Model& otherModel)
	{
		for (const auto otherAnimPair : otherModel.mAnimations)
		{
			const AnimationID id = otherAnimPair.first;
			const Animation& otherAnimation = otherAnimPair.second;
			
			mAnimations.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(otherAnimation, mParentMap, mBoneOffsetTransforms));
		}
	}


    uint32_t CountNumMeshes(const PackageModel& model)
    {
        uint32_t numMeshes = 0;

        for (const PackageNode& node : model.mNodes)
            numMeshes += static_cast<uint32_t>( node.mMeshes.size() );

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

    // inefficient, but not a big concern, since its only used during initalization
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

    void ReserveStorage(const PackageModel& model, ModelNode::NodeContainer& nodeContainer, ModelNode::MeshContainer& meshContainer, Model::AnimationMap& animationMap, Model::AnimationNameMap& animationNameMap)
    {
        // count meshes/nodes and reserve() space in containers before parsing them
        // otherwise iterators gets invalidated as parsing goes on and containers grow
        const uint32_t numMeshes = CountNumMeshes(model);
        const uint32_t numNodes = static_cast<uint32_t>( model.mNodes.size() );
		const uint32_t numAnimations = static_cast<uint32_t>( model.mAnimations.size() );

        meshContainer.reserve(numMeshes);
        nodeContainer.reserve(numNodes);
		animationMap.reserve(numAnimations);
		animationNameMap.reserve(numAnimations);
    }
}