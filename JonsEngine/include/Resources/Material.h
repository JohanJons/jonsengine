#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>

namespace JonsEngine
{
    class Material
    {
    public:
        Material(const std::string& name, const DX11MaterialID diffuseTexture, const DX11MaterialID normalTexture, const Vec3& diffuseColor, const Vec3& ambientColor, const Vec3& specularColor, const Vec3& emissiveColor, const float specularFactor);
        ~Material();


        const std::string mName;
        const DX11MaterialID mDiffuseTexture;
        const DX11MaterialID mNormalTexture;
        const Vec3 mDiffuseColor;
        const Vec3 mAmbientColor;
        const Vec3 mSpecularColor;
        const Vec3 mEmissiveColor;
        const float mSpecularFactor;
    };

    typedef IDMap<Material>::ItemID MaterialID;
    static const MaterialID INVALID_MATERIAL_ID = IDMap<Material>::INVALID_ITEM_ID;
}