#include "include/Scene/ModelNode.h"

#include "include/Resources/JonsPackage.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
#include "include/Core/Utils/Math.h"

namespace JonsEngine
{
    ModelNode::ModelNode(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material) :
        mName(name), mTransform(initialTransform), mAABBCenter(0.5f * (minBounds + maxBounds)), mAABBExtent(0.5f * (maxBounds - minBounds))
    {
		mMeshes.emplace_back(name, minBounds, maxBounds, meshID, material);
    }

    ModelNode::ModelNode(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageNode& node, LoadMaterialFunc loadMaterialFunction) :
        mName(node.mName), mTransform(node.mTransform), mAABBCenter(0.5f * (node.mAABB.mMinBounds + node.mAABB.mMaxBounds)), mAABBExtent(0.5f * (node.mAABB.mMaxBounds - node.mAABB.mMinBounds))
    {
		for (const PackageMesh& mesh : node.mMeshes)
		{
			const MeshID meshID = renderer.CreateMesh(mesh.mVertexData, mesh.mNormalData, mesh.mTexCoordsData, mesh.mTangents, mesh.mBitangents, mesh.mIndiceData, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
			
			MaterialPtr material = nullptr;
			if (mesh.mHasMaterial)
				material = loadMaterialFunction(jonsPkg->mMaterials.at(mesh.mMaterialIndex).mName, jonsPkg);

			mMeshes.emplace_back(mesh.mName, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds, meshID, material);
		}

		for (const PackageNode& node : node.mChildNodes)
			mChildNodes.emplace_back(renderer, jonsPkg, node, loadMaterialFunction);
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
}