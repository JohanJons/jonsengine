#include "include/Scene/Model.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Model::Model(const std::string& name) : mName(name), mHashedID(boost::hash_value(name)), mLightingEnabled(true), mMaterialTilingFactor(1.0f)
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