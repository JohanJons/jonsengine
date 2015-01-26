#include "include/Scene/Model.h"

#include "include/Core/Utils/Math.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds) : mName(name), mHashedID(boost::hash_value(name)), mRootNode(name, initialTransform, minBounds, maxBounds)
    {
    }

    Model::Model(const PackageModel& pkgModel) : mName(pkgModel.mName), mHashedID(boost::hash_value(pkgModel.mName)), mRootNode(pkgModel.mRootNode)
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


	ModelNode& Model::GetRootNode()
	{
		return mRootNode;
	}

		
	const std::string& Model::GetName() const
	{
		return mName;
	}

    const size_t Model::GetHashedName() const
    {
        return mHashedID;
    }
}