#pragma once

#include "include/Scene/Material.h"
#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class Mesh
    {
    public:
        Mesh(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const DX11MeshID meshID, MaterialPtr material);
        ~Mesh();

		float GetTextureTilingFactor() const;
        void SetTextureTilingFactor(const float tilingFactor);

		MaterialPtr GetMaterial() const;
		void SetMaterial(MaterialPtr material);


        const std::string mName;
        const DX11MeshID mMeshID;
        const AABB mLocalAABB;


    private:
		MaterialPtr mMaterial;
		float mMaterialTilingFactor;
    };
}