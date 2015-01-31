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


        const std::string mName;
        const MeshID mMeshID;

        const Vec3 mAABBCenter;
        const Vec3 mAABBExtent;

        MaterialPtr mMaterial;
        float mMaterialTilingFactor;


    private:
    };
}