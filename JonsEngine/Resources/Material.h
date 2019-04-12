#pragma once

#include "Types.h"
#include "IDMap.hpp"

#include <string>

namespace JonsEngine
{
    class Material
    {
    public:
		static constexpr float DEFAULT_DIFFUSE_COLOR = 1.0f;
		static constexpr float DEFAULT_AMBIENT_COLOR = 1.0f;
		static constexpr float DEFAULT_SPECULAR_COLOR = 1.0f;
		static constexpr float DEFAULT_EMISSIVE_COLOR = 1.0f;
		static constexpr float DEFAULT_SPECULAR_FACTOR = 0.02f;

		Material(const std::string& name, const DX11TextureID diffuseTexture, const DX11TextureID normalTexture);
        Material(const std::string& name, const DX11TextureID diffuseTexture, const DX11TextureID normalTexture, const Vec3& diffuseColor, const Vec3& ambientColor, const Vec3& specularColor, const Vec3& emissiveColor, const float specularFactor);
        ~Material();

        const std::string& GetName() const;
        DX11TextureID GetDiffuseTexture() const;
        DX11TextureID GetNormalTexture() const;
        float GetSpecularFactor() const;


    private:
         std::string mName;
         DX11TextureID mDiffuseTexture;
         DX11TextureID mNormalTexture;
         Vec3 mDiffuseColor;
         Vec3 mAmbientColor;
         Vec3 mSpecularColor;
         Vec3 mEmissiveColor;
         float mSpecularFactor;
    };

    typedef IDMap<Material>::ItemID MaterialID;
    static const MaterialID INVALID_MATERIAL_ID = IDMap<Material>::INVALID_ITEM_ID;
}