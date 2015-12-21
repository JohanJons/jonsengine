#pragma once

#include "include/Resources/ModelAnimation.h"
#include "include/Resources/ModelNode.h"
#include "include/Resources/Mesh.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Containers/RangedIterator.hpp"

#include <string>
#include <vector>

namespace JonsEngine
{
    typedef uint32_t ModelAnimationIndex;
    static const ModelAnimationIndex INVALID_ANIMATION_INDEX = std::numeric_limits<ModelAnimationIndex>::max();

    class Model
    {
    private:
        typedef std::vector<ModelAnimation> AnimationContainer;
        typedef std::vector<Mesh> MeshContainer;
        typedef std::vector<ModelNode> NodeContainer;

    public:
        typedef ConstRangedIterator<AnimationContainer, AnimationContainer::const_iterator> ModelAnimationIterator;
        typedef ConstRangedIterator<MeshContainer, MeshContainer::const_iterator> MeshIterator;
        typedef ConstRangedIterator<NodeContainer, NodeContainer::const_iterator> NodeIterator;

        Model(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID);
        Model(const PackageModel& pkgModel, const ModelNode::InitDataList& initData);
        Model(const Model& other);
        ~Model();

        MeshIterator GetMeshes() const;
        NodeIterator GetNodes() const;
        ModelAnimationIterator GetAnimations() const;

        ModelAnimationIndex GetAnimationIndex(const std::string& name) const;
        const ModelAnimation& GetAnimation(const ModelAnimationIndex animationIndex) const;

        const ModelNode& GetRootNode() const;
        const std::string& GetName() const;
        const AABB& GetStaticAABB() const;
        

    private:
        void ParseNodes(const PackageModel& model, const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode, const ModelNode::NodeIterator& next);
        void ParseNodes(const Model& other);
        ModelNode::MeshIterator ParseMeshes(const ModelNode::InitDataList& initDataList, const PackageNode& pkgNode);


        std::string mName;
        AABB mStaticAABB;
        NodeContainer mNodes;
        MeshContainer mMeshes;
        AnimationContainer mAnimations;
    };

    typedef IDMap<Model>::ItemID ModelID;
    static const ModelID INVALID_MODEL_ID = IDMap<Model>::INVALID_ITEM_ID;
}