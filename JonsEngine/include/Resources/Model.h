#pragma once

#include "include/Resources/Animation.h"
#include "include/Resources/ModelNode.h"
#include "include/Resources/Mesh.h"
#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/RangedIterator.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace JonsEngine
{
	struct PackageModel;
	struct PackageNode;

    class Model
    {
    private:
        typedef std::vector<Mesh> MeshContainer;
        typedef std::vector<ModelNode> NodeContainer;

    public:
        typedef ConstRangedIterator<MeshContainer, MeshContainer::const_iterator> MeshIterator;
        typedef ConstRangedIterator<NodeContainer, NodeContainer::const_iterator> NodeIterator;
        typedef std::unordered_map<AnimationID, Animation> AnimationMap;
		typedef std::unordered_map<std::string, AnimationID> AnimationNameMap;
        typedef ConstRangedIterator<AnimationMap, AnimationMap::const_iterator> AnimationIterator;

        Model(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID);
        Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData);
        Model(const Model& other);

		bool HasAnimations() const;

        MeshIterator GetMeshes() const;
        NodeIterator GetNodes() const;
        AnimationIterator GetAnimations() const;

        AnimationID GetAnimationID(const std::string& name) const;
		const Animation& GetAnimation(const AnimationID id) const;

        const ModelNode& GetRootNode() const;
        const std::string& GetName() const;
        const AABB& GetStaticAABB() const;
        

    private:
        void ParseNodes(const PackageModel& model, const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode, const ModelNode::NodeIterator& next);
        ModelNode::MeshIterator ParseMeshes(const PackageModel& model, const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode);
		void ParseSkeleton(const PackageModel& model);
		void ParseAnimations(const PackageModel& pkgModel);

		void CopyNodeHierarchy(const Model& otherModel);
		void CopyAnimationHierarchy(const Model& otherModel);


        std::string mName;
        AABB mStaticAABB;
        NodeContainer mNodes;
        MeshContainer mMeshes;
		AnimationMap mAnimations;
		AnimationNameMap mAnimationNameMap;
		BoneParentMap mParentMap;
		BoneTransforms mBoneOffsetTransforms;
		AnimationIDGenerator mAnimationIDGen;
    };

    typedef IDMap<Model>::ItemID ModelID;
    static const ModelID INVALID_MODEL_ID = IDMap<Model>::INVALID_ITEM_ID;
}