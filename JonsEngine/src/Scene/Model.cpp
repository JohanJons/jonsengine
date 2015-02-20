#include "include/Scene/Model.h"

#include "include/Core/Utils/Math.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
	ModelNode::Index AddModelNode(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, LoadMaterialFunc loadMaterialFunction, ModelDataCollection& modelData, const PackageNode& node)
	{
		modelData.mAABBs.emplace_back(0.5f * (node.mAABB.mMinBounds + node.mAABB.mMaxBounds), 0.5f * (node.mAABB.mMaxBounds - node.mAABB.mMinBounds));
		modelData.mTransforms.emplace_back(node.mTransform);

		auto meshesIndexBegin = modelData.mMeshes.size();
		for (const PackageMesh& mesh : node.mMeshes)
		{
			const MeshID meshID = renderer.CreateMesh(mesh.mVertexData, mesh.mNormalData, mesh.mTexCoordsData, mesh.mTangents, mesh.mBitangents, mesh.mIndiceData, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);

			MaterialPtr material = nullptr;
			if (mesh.mHasMaterial)
				material = loadMaterialFunction(jonsPkg->mMaterials.at(mesh.mMaterialIndex).mName, jonsPkg);

			modelData.mMeshes.emplace_back(mesh.mName, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds, meshID, material);
		}
		auto meshesIndexEnd = modelData.mMeshes.size();

		// err, not layed out as planned in memory...
		auto childIndexBegin = modelData.mNodes.size();
		uint32_t numChildren = 0;
		for (const PackageNode& child : node.mChildNodes)
		{
			modelData.mNodes.emplace_back(renderer, jonsPkg, loadMaterialFunction, modelData, child);
			numChildren++;
		}
		auto childIndexEnd = childIndexBegin + numChildren;

		modelData.mNodes.emplace_back(childIndexBegin, childIndexEnd, meshesIndexBegin, meshesIndexEnd, modelData.mAABBs.size() - 1, modelData.mTransforms.size() - 1);

		return modelData.mNodes.size() - 1;
	}

	ModelNode::Index AddModelNode(ModelDataCollection& modelData, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material)
	{
		modelData.mAABBs.emplace_back(0.5f * (minBounds + maxBounds), 0.5f * (maxBounds - minBounds));
		modelData.mTransforms.emplace_back(initialTransform);

		auto meshesIndexBegin = modelData.mMeshes.size();
		modelData.mMeshes.emplace_back(meshID, material, modelData.mAABBs.size() - 1, modelData.mTransforms.size() - 1);
		auto meshesIndexEnd = modelData.mMeshes.size();

		// range less than one (0 - 0) means no children
		modelData.mNodes.emplace_back(0, 0, meshesIndexBegin, meshesIndexEnd, modelData.mAABBs.size() - 1, modelData.mTransforms.size() - 1);
	}


	ModelMesh::ModelMesh(const MeshID meshID, MaterialPtr ptr, const AABB::Index aabbIndex, const Transform::Index transformIndex) : mMeshID(meshID), mMaterial(ptr), mAABBIndex(aabbIndex), mTransformIndex(transformIndex)
    {
    }


	ModelNode::ModelNode(const ModelNode::Index childrenBegin, const ModelNode::Index childrenEnd, const ModelMesh::Index meshesBegin, const ModelMesh::Index meshesEnd, const AABB::Index aabb, const Transform::Index transform) :
		mChildrenBegin(childrenBegin), mChildrenEnd(childrenEnd), mMeshesBegin(meshesBegin), mMeshesEnd(meshesEnd), mAABB(aabb), mTransform(transform)
    {
    }


	Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material, ModelDataCollection& modelData) :
		mName(name), mHashedID(boost::hash_value(name)), mRootNode(AddModelNode(modelData, initialTransform, minBounds, maxBounds, meshID, material))
    {
    }


    Model::Model(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageModel& pkgModel, LoadMaterialFunc loadMaterialFunction, ModelDataCollection& modelData) :
		mName(pkgModel.mName), mHashedID(boost::hash_value(pkgModel.mName)), mRootNode(AddModelNode(renderer, jonsPkg, loadMaterialFunction, modelData, pkgModel.mRootNode))
    {
    }

    Model::~Model()
    {
    }


	bool Model::operator==(const Model& m1)
    {
        return mHashedID == m1.mHashedID;
    }
        
    bool Model::operator==(const std::string& modelName)
    {
        return mHashedID == boost::hash_value(modelName);
    }
}