#pragma once

#include "include/Core/Types.h"

#include <memory>
#include <functional>

namespace JonsEngine
{
    class Material
    {
	public:
		Material(const std::string& name, const MaterialID diffuseTexture, const MaterialID normalTexture, const Vec3& diffuseColor, const Vec3& ambientColor, const Vec3& specularColor, const Vec3& emissiveColor, const float specularFactor);

        bool operator==(const Material& material);
        bool operator==(const std::string& materialName);

		const std::string mName;
		const MaterialID mDiffuseTexture;
        const MaterialID mNormalTexture;
        const Vec3 mDiffuseColor;
        const Vec3 mAmbientColor;
        const Vec3 mSpecularColor;
        const Vec3 mEmissiveColor;
        const float mSpecularFactor;
		

	private:
        const size_t mHashedID;
    };

	typedef std::shared_ptr<Material> MaterialPtr;
}