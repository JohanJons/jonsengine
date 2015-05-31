#include "include/Scene/ModelNode.h"

#include "include/Resources/JonsPackage.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"

namespace JonsEngine
{
    ModelNode::ModelNode(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material, IDMap<Mat4>& transformStorage) :
        mName(name), mLocalTransform(transformStorage, initialTransform), mLocalAABB(minBounds, maxBounds)
    {
		mMeshes.emplace_back(name, minBounds, maxBounds, meshID, material);
    }

    ModelNode::ModelNode(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageNode& node, const Mat4& parentTransform, LoadMaterialFunc loadMaterialFunction, IDMap<Mat4>& transformStorage) :
        mName(node.mName), mLocalTransform(transformStorage, parentTransform * node.mTransform), mLocalAABB(node.mAABB.mMinBounds, node.mAABB.mMaxBounds)
    {
		for (const PackageMesh& mesh : node.mMeshes)
		{
			const MeshID meshID = renderer.CreateMesh(mesh.mVertexData, mesh.mNormalData, mesh.mTexCoordsData, mesh.mTangents, mesh.mBitangents, mesh.mIndiceData, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
			
			MaterialPtr material = nullptr;
			if (mesh.mHasMaterial)
				material = loadMaterialFunction(jonsPkg->mMaterials.at(mesh.mMaterialIndex).mName, jonsPkg);

            mMeshes.emplace_back(mesh.mName, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds, meshID, material);
		}

        // save absolue model transforms rather than relative to parent. 
        // TODO: this might cause trouble when doing animations?
		for (const PackageNode& child : node.mChildNodes)
            mChildNodes.emplace_back(renderer, jonsPkg, child, GetLocalTransform(), loadMaterialFunction, transformStorage);
    }

    ModelNode::ModelNode(ModelNode&& other) throw() :
        mName(std::move(other.mName)), mLocalAABB(std::move(other.mLocalAABB)), mLocalTransform(std::move(other.mLocalTransform)), mMeshes(std::move(other.mMeshes)), mChildNodes(std::move(other.mChildNodes))
    {
    }

    ModelNode::~ModelNode()
    {
    }


	std::vector<Mesh>& ModelNode::GetMeshes()
	{
		return mMeshes;
	}

	std::vector<ModelNode>& ModelNode::GetChildNodes()
	{
		return mChildNodes;
	}


    const Mat4& ModelNode::GetLocalTransform() const
    {
        return mLocalTransform.Get();
    }

    IDMap<Mat4>::ItemID ModelNode::GetLocalTransformID() const
    {
        return mLocalTransform.GetID();
    }
}