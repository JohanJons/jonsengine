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
    class ModelNode
    {
    public:
        typedef std::tuple<const PackageMesh&, const DX11MeshID, const MaterialID> InitData;
        typedef std::vector<InitData> InitDataList;

        typedef std::vector<ModelNode> NodeContainer;
        typedef std::vector<Mesh> MeshContainer;

        typedef ConstRangedIterator<MeshContainer> MeshIterator;
        typedef ConstRangedIterator<NodeContainer> AllChildrenIterator;

    private:
        typedef std::vector<ModelNode>::const_iterator NodeIterator;

        // this iterator is defined because traversing immediate children must be different since the node tree is built depth-first
        class ImmediateChildrenIter : public NodeIterator
        {
        public:
            ImmediateChildrenIter(const NodeIterator& iter);

            ImmediateChildrenIter& operator++();
        };

    public:
        typedef ConstRangedIterator<NodeContainer, ImmediateChildrenIter> ImmediateChildrenIterator;

        ModelNode(const PackageNode& pkgNode, const ImmediateChildrenIterator& immChildIter, const AllChildrenIterator& childIter, const MeshIterator& meshIter);
        ModelNode(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const Mat4& initialTransform, const ImmediateChildrenIterator& immChildIter, const AllChildrenIterator& allChildIter, const MeshIterator& meshIter);
        ~ModelNode();


        const std::string mName;
        const AABB mLocalAABB;
        const Mat4 mLocalTransform;

        const ImmediateChildrenIterator mImmediateChildNodes;
        const AllChildrenIterator mAllChildNodes;
        const MeshIterator mMeshes;


    private:
        friend class Model;

        template <typename ChildIterator>
        ChildIterator ParseChildNodes(const PackageNode& pkgNode, const InitDataList& initData, NodeContainer& nodes, MeshContainer& meshes);

        // points to the next sibling in the same node depth level or nullptr if none available
        ModelNode* mNext;
    };
}