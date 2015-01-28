#include "include/Scene/Material.h"

#include "boost/functional/hash.hpp"

namespace JonsEngine
{
    Material::Material(const std::string& name, const TextureID diffuseTexture, const TextureID normalTexture, const Vec3& diffuseColor, const Vec3& ambientColor, const Vec3& specularColor, const Vec3& emissiveColor, const float specularFactor) :
        mName(name), mHashedID(boost::hash_value(name)), mDiffuseTexture(diffuseTexture), mNormalTexture(normalTexture), mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor), mEmissiveColor(emissiveColor), mSpecularFactor(specularFactor)
    {
		// TODO: actual material props. for shading if any models or materials needs it
		assert(glm::length(diffuseColor) == 0);
		assert(glm::length(ambientColor) == 0);
		assert(glm::length(specularColor) == 0);
		assert(glm::length(emissiveColor) == 0);
    }


    bool Material::operator==(const Material& m1)
    {
        return mHashedID == m1.mHashedID;
    }
        
    bool Material::operator==(const std::string& materialName)
    {
        return mHashedID == boost::hash_value(materialName);
    }
}