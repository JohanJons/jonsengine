#include "include/Scene/Model.h"

#include "include/Core/Utils/Math.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Model::Model(const std::string& name, const Mat4& initialTransform, const Vec3& minBounds, const Vec3& maxBounds) : 
        mName(name), mHashedID(boost::hash_value(name)), mInitialTransform(initialTransform), mAABBCenter(0.5f * (minBounds + maxBounds)), mAABBExtent(0.5f * (maxBounds - minBounds))
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


    const Vec3& Model::GetAABBCenter() const
    {
        return mAABBCenter;
    }

    const Vec3& Model::GetAABBExtent() const
    {
        return mAABBExtent;
    }


	void Model::SetSceneNode(SceneNodePtr node)
	{
		mSceneNode = node;
	}
		
	SceneNodePtr Model::GetSceneNode()
	{
		return mSceneNode;
	}

		
	const std::string& Model::GetName() const
	{
		return mName;
	}
		
	const Mat4& Model::GetInitialTransform() const
	{
		return mInitialTransform;
	}
		
	std::vector<ModelNode>& Model::GetModelNodes()
	{
		return mNodes;
	}
}