#pragma once

#include "Resources/Animation.h"
#include "Resources/ModelNode.h"
#include "Resources/Mesh.h"
#include "Core/Types.h"
#include "Core/Math/AABB.h"
#include "Core/Containers/IDMap.hpp"
#include "Core/Containers/RangedIterator.hpp"

#include <string>
#include <vector>
#include <unordered_map>

namespace JonsEngine
{
	struct PackageModel;
	struct PackageNode;

    class Model
    {
    public:
        typedef std::vector<Mesh> MeshContainer;
        typedef std::vector<ModelNode> NodeContainer;
        typedef ConstRangedIterator<MeshContainer> MeshIterator;
        typedef ConstRangedIterator<NodeContainer> NodeIterator;

        typedef std::unordered_map<AnimationID, Animation> AnimationMap;
		typedef std::unordered_map<std::string, AnimationID> AnimationNameMap;
        typedef ConstRangedIterator<AnimationMap> AnimationIterator;

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
        void ParseNodes( const PackageModel& model, const ModelNode::InitDataList& initDataList, const std::vector<PackageNode>& pkgNodes );
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