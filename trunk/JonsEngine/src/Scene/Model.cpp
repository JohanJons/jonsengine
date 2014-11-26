#include "include/Scene/Model.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Model::Model(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds) : mName(name), mHashedID(boost::hash_value(name)), mLightingEnabled(true), mMesh(INVALID_MESH_ID), mMaterialTilingFactor(1.0f),
        mAABBCenter(0.5f * (minBounds + maxBounds)), mAABBExtent(0.5f * (maxBounds - minBounds))
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