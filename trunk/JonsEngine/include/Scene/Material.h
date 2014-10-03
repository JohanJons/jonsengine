#pragma once

#include "include/Core/Types.h"

#include <memory>

namespace JonsEngine
{
    struct Material;

    typedef std::shared_ptr<Material> MaterialPtr;

    struct Material
    {
        const std::string mName;
        const size_t mHashedID;

        TextureID mDiffuseTexture;
        TextureID mNormalTexture;
        Vec3 mDiffuseColor;
        Vec3 mAmbientColor;
        Vec3 mSpecularColor;
        Vec3 mEmissiveColor;
        float mSpecularFactor;

        Material(const std::string& name, const TextureID diffuseTexture, const TextureID normalTexture, const Vec3& diffuseColor, const Vec3& ambientColor, const Vec3& specularColor, const Vec3& emissiveColor, const float specularFactor);

        bool operator==(const Material& material);
        bool operator==(const std::string& materialName);
    };
}