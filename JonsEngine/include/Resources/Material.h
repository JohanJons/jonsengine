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

        const std::string& GetName() const;
        DX11MaterialID GetDiffuseTexture() const;
        DX11MaterialID GetNormalTexture() const;
        float GetSpecularFactor() const;


    private:
         std::string mName;
         DX11MaterialID mDiffuseTexture;
         DX11MaterialID mNormalTexture;
         Vec3 mDiffuseColor;
         Vec3 mAmbientColor;
         Vec3 mSpecularColor;
         Vec3 mEmissiveColor;
         float mSpecularFactor;
    };

    typedef IDMap<Material>::ItemID MaterialID;
    static const MaterialID INVALID_MATERIAL_ID = IDMap<Material>::INVALID_ITEM_ID;
}