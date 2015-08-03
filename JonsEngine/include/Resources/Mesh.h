#pragma once

#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

#include <string>

namespace JonsEngine
{
    class Mesh
    {
    public:
        Mesh(const std::string name, const DX11MeshID meshID, const DX11MaterialID materialID, const Vec3& minBounds, const Vec3& maxBounds);
        ~Mesh();

        const std::string mName;
        const DX11MeshID mMeshID;
        const AABB mLocalAABB;

        DX11MaterialID mMaterialID;
        float mMaterialTilingFactor;
    };
}