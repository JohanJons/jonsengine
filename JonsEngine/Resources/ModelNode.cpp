#include "Resources/ModelNode.h"

#include <algorithm>

namespace JonsEngine
{
    uint32_t CountNumMeshes(const ModelNode::MeshIterator& meshIter);

    ModelNode::ModelNode(const PackageNode& pkgNode, const MeshIterator& meshIter) :
        mName(pkgNode.mName),
		mNodeIndex(pkgNode.mNodeIndex),
		mLocalTransform(pkgNode.mTransform),
		mNumMeshes(CountNumMeshes(meshIter)),
        mMeshes(meshIter)
    {
    }

    ModelNode::ModelNode(const std::string& name, const ModelNodeIndex nodeIndex, const Mat4& localTransform, const MeshIterator& meshIter) :
        mName(name),
		mNodeIndex(nodeIndex),
		mLocalTransform(localTransform),
		mNumMeshes(CountNumMeshes(meshIter)),
		mMeshes(meshIter)
    {
    }

    uint32_t ModelNode::GetNumMeshes() const
    {
        return mNumMeshes;
    }

    const std::string& ModelNode::GetName() const
    {
        return mName;
    }

    ModelNodeIndex ModelNode::GetModelNodeIndex() const
    {
        return mNodeIndex;
    }

	const Mat4& ModelNode::GetLocalTransform() const
	{
		return mLocalTransform;
	}


    ModelNode::MeshIterator ModelNode::GetMeshes() const
    {
        return mMeshes;
    }

    uint32_t CountNumMeshes(const ModelNode::MeshIterator& meshIter)
    {
        uint32_t ret = 0;

        for (const Mesh& mesh : meshIter)
            ++ret;

        return ret;
    }
}