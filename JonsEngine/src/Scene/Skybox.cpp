#include "include/Scene/Skybox.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Skybox::Skybox(const std::string& name, const MaterialID skyboxTexture) : mName(name), mSkyboxTexture(skyboxTexture), mHashedID(boost::hash_value(name))
    {
    }


    bool Skybox::operator==(const Skybox& otherSkybox)
    {
        return mHashedID == otherSkybox.mHashedID;
    }

    bool Skybox::operator==(const std::string& skyboxName)
    {
        return mHashedID == boost::hash_value(skyboxName);
    }
}