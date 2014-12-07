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

        const Vec3& GetAABBCenter() const;
        const Vec3& GetAABBExtent() const;


        const std::string mName;
        const MeshID mMeshID;

        MaterialPtr mMaterial;
        float mMaterialTilingFactor;


    private:
        Vec3 mAABBCenter;
        Vec3 mAABBExtent;
    };
}