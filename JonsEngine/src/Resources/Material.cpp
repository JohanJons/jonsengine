#include "include/Resources/Material.h"

namespace JonsEngine
{
    Material::Material(const std::string& name, const DX11MaterialID diffuseTexture, const DX11MaterialID normalTexture, const Vec3& diffuseColor, const Vec3& ambientColor, const Vec3& specularColor, const Vec3& emissiveColor, const float specularFactor) :
        mName(name), mDiffuseTexture(diffuseTexture), mNormalTexture(normalTexture), mDiffuseColor(diffuseColor), mAmbientColor(ambientColor), mSpecularColor(specularColor), mEmissiveColor(emissiveColor), mSpecularFactor(specularFactor)
    {
        // TODO: actual material props. for shading if any models or materials needs it
    }
    
    Material::~Material()
    {
    }


    const std::string& Material::GetName() const
    {
        return mName;
    }

    DX11MaterialID Material::GetDiffuseTexture() const
    {
        return mDiffuseTexture;
    }

    DX11MaterialID Material::GetNormalTexture() const
    {
        return mNormalTexture;
    }

    float Material::GetSpecularFactor() const
    {
        return mSpecularFactor;
    }
}