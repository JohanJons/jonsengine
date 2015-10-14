#pragma once

#include "include/Resources/Mesh.h"
#include "include/Resources/JonsPackage.h"
#include "include/Core/Containers/RangedIterator.hpp"
#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

#include <vector>
#include <string>
#include <tuple>

namespace JonsEngine
{
    typedef PackageNode::PackageNodeID ModelNodeID;
    static const ModelNodeID INVALID_MODEL_NODE_ID = PackageNode::INVALID_NODE_ID;

    class ModelNode
    {
    public:
        typedef std::tuple<const PackageMesh&, const DX11MeshID, const MaterialID> InitData;
        typedef std::vector<InitData> InitDataList;

        typedef std::vector<ModelNode> NodeContainer;
        typedef std::vector<Mesh> MeshContainer;

        typedef ConstRangedIterator<MeshContainer> MeshIterator;
        typedef ConstRangedIterator<NodeContainer> AllChildrenIterator;
        typedef NodeContainer::const_iterator NodeIterator;

    private:
        // this iterator is defined because traversing immediate children must be different since the node tree is built depth-first
        // some standard iterator operators are deleted to make it play nicer
        // not a perfect solution but good enough for the limited purpose at hand
        class ImmediateChildrenIter : public NodeIterator
        {
        public:
            ImmediateChildrenIter(const NodeIterator& iter);

            ImmediateChildrenIter& operator++();

            ImmediateChildrenIter& operator--() = delete;
            ImmediateChildrenIter operator--(int) = delete;
            ImmediateChildrenIter& operator+=(difference_type) = delete;
            ImmediateChildrenIter operator+(difference_type) const = delete;
            ImmediateChildrenIter& operator-=(difference_type) = delete;
        };

    public:
        typedef ConstRangedIterator<NodeContainer, ImmediateChildrenIter> ImmediateChildrenIterator;

        ModelNode(const PackageNode& pkgNode, const Mat4& parentTransform, const ImmediateChildrenIterator& immChildIter, const AllChildrenIterator& childIter, const MeshIterator& meshIter, const NodeIterator& next);
        ModelNode(const std::string& name, const ModelNodeID nodeID, const Vec3& minBounds, const Vec3& maxBounds, const Mat4& initialTransform, const ImmediateChildrenIterator& immChildIter, const AllChildrenIterator& allChildIter,
            const MeshIterator& meshIter, const NodeIterator& next);
        ~ModelNode();

        const std::string& GetName() const;
        const ModelNodeID GetModelNodeID() const;
        const AABB& GetLocalAABB() const;
        const Mat4& GetLocalTransform() const;

        MeshIterator& GetMeshes() const;
        AllChildrenIterator& GetAllChildren() const;
        ImmediateChildrenIterator& GetImmediateChildren() const;


    private:
        std::string mName;
        ModelNodeID mNodeID;
        AABB mLocalAABB;
        Mat4 mLocalTransform;

        ImmediateChildrenIterator mImmediateChildNodes;
        AllChildrenIterator mAllChildNodes;
        MeshIterator mMeshes;

        // points to the next sibling in the same node depth level or end() if none available
        NodeIterator mNext;
    };
}