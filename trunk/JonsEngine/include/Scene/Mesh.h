#pragma once

#include "include/Scene/Material.h"
#include "include/Core/Types.h"

#include <string>
#include <vector>

namespace JonsEngine
{
    class Mesh
    {
    public:
        Mesh(const std::string& name, const Vec3& minBounds, const Vec3& maxBounds, const MeshID meshID, MaterialPtr material);
        ~Mesh();

		float GetTextureTilingFactor() const;
        void SetTextureTilingFactor(const float tilingFactor);

		MaterialPtr GetMaterial() const;
		void SetMaterial(MaterialPtr material);


        const std::string mName;
        const MeshID mMeshID;

        const Vec3 mAABBCenter;
        const Vec3 mAABBExtent;


    private:
		MaterialPtr mMaterial;
		float mMaterialTilingFactor;
    };
}