#pragma once

#include "include/Core/Types.h"
#include "boost/shared_ptr.hpp"

namespace JonsEngine
{
    struct Material;

    /* MaterialPtr definition */
    typedef boost::shared_ptr<Material> MaterialPtr;

    /* Material defintion */
    struct Material
    {
        const std::string mName;
        const size_t mHashedID;

        TextureID mDiffuseTexture;
        Vec3 mDiffuseColor;
        Vec3 mAmbientColor;
        Vec3 mSpecularColor;
        Vec3 mEmissiveColor;
        float mSpecularFactor;

        Material(const std::string& name, const TextureID diffuseTexture, const Vec3& diffuseColor, const Vec3& ambientColor, const Vec3& specularColor, const Vec3& emissiveColor);

        bool operator==(const Material& material);
        bool operator==(const std::string& materialName);
    };
}