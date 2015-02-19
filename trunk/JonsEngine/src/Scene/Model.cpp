#include "include/Scene/Model.h"

#include "include/Core/Utils/Math.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    ModelMesh::ModelMesh(const MeshID meshID, const uint32_t aabbIndex, const uint32_t transformIndex) : mMeshID(meshID), mAABBIndex(aabbIndex), mTransformIndex(transformIndex)
    {
    }


    ModelNode::ModelNode(const uint32_t aabbIndex, const uint32_t transformIndex) : mAABBIndex(aabbIndex), mTransformIndex(transformIndex)
    {
    }


    Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material) :
		mName(name), mHashedID(boost::hash_value(name))//, mRootNode(name, initialTransform, minBounds, maxBounds, meshID, material)
    {
    }


    Model::Model(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageModel& pkgModel, LoadMaterialFunc loadMaterialFunction, ModelDataCollection& modelData) :
		mName(pkgModel.mName), mHashedID(boost::hash_value(pkgModel.mName))//, mRootNode(renderer, jonsPkg, pkgModel.mRootNode, loadMaterialFunction)
    {
        mAABBs.emplace_back(0.5f * (pkgModel.mRootNode.mAABB.mMinBounds + pkgModel.mRootNode.mAABB.mMaxBounds), 0.5f * (pkgModel.mRootNode.mAABB.mMaxBounds - pkgModel.mRootNode.mAABB.mMinBounds));
        mTransforms.emplace_back(pkgModel.mRootNode.mTransform);
        mNodes.emplace_back(mAABBs.size() - 1, mTransforms.size() - 1);
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