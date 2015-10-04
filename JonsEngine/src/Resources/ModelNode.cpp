#include "include/Resources/ModelNode.h"

#include <algorithm>

namespace JonsEngine
{
    //
    // ModelNode::ImmediateChildrenIter
    //

    ModelNode::ImmediateChildrenIter::ImmediateChildrenIter(const NodeIterator& iter) : NodeIterator(iter)
    {
    }

    ModelNode::ImmediateChildrenIter& ModelNode::ImmediateChildrenIter::operator++()
    {
        this->_Ptr = this->_Ptr->mNext._Ptr;

        return *this;
    }



    //
    // ModelNode
    //

    ModelNode::ModelNode(const PackageNode& pkgNode, const Mat4& parentTransform, const ImmediateChildrenIterator& immChildIter, const AllChildrenIterator& childIter, const MeshIterator& meshIter, const NodeIterator& next) :
        mName(pkgNode.mName), mNodeID(pkgNode.mNodeID), mLocalAABB(pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds), mLocalTransform(parentTransform * Mat4(1.0f)), mImmediateChildNodes(immChildIter), mAllChildNodes(childIter),
        mMeshes(meshIter), mNext(next)
    {
    }

    ModelNode::ModelNode(const std::string& name, const ModelNodeID nodeID, const Vec3& minBounds, const Vec3& maxBounds, const Mat4& initialTransform, const ImmediateChildrenIterator& immChildIter, const AllChildrenIterator& allChildIter,
        const MeshIterator& meshIter, const NodeIterator& next) :
        mName(name), mNodeID(nodeID), mLocalAABB(minBounds, maxBounds), mLocalTransform(Mat4(1.0f)), mImmediateChildNodes(immChildIter), mAllChildNodes(allChildIter), mMeshes(meshIter), mNext(next)
    {
    }

    ModelNode::~ModelNode()
    {
    }


    const std::string& ModelNode::GetName() const
    {
        return mName;
    }

    const ModelNodeID ModelNode::GetModelNodeID() const
    {
        return mNodeID;
    }

    const AABB& ModelNode::GetLocalAABB() const
    {
        return mLocalAABB;
    }

    const Mat4& ModelNode::GetLocalTransform() const
    {
        return mLocalTransform;
    }


    const ModelNode::MeshIterator& ModelNode::GetMeshes() const
    {
        return mMeshes;
    }

    const ModelNode::AllChildrenIterator& ModelNode::GetAllChildren() const
    {
        return mAllChildNodes;
    }

    const ModelNode::ImmediateChildrenIterator& ModelNode::GetImmediateChildren() const
    {
        return mImmediateChildNodes;
    }
}