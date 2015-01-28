#include "include/Scene/Model.h"

#include "include/Core/Utils/Math.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material) :
		mName(name), mHashedID(boost::hash_value(name)), mRootNode(name, initialTransform, minBounds, maxBounds, meshID, material)
    {
    }

    Model::Model(DX11Renderer& renderer, const JonsPackagePtr jonsPkg, const PackageModel& pkgModel, LoadMaterialFunc loadMaterialFunction) :
		mName(pkgModel.mName), mHashedID(boost::hash_value(pkgModel.mName)), mRootNode(renderer, jonsPkg, pkgModel.mRootNode, loadMaterialFunction)
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