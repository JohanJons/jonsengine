#pragma once

#include "include/Resources/Material.h"
#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

#include <string>

namespace JonsEngine
{
    class Mesh
    {
    public:
        Mesh(const std::string& name, const DX11MeshID meshID, const MaterialID defaultMaterialID, const Vec3& minBounds, const Vec3& maxBounds);
        ~Mesh();


        const std::string mName;
        const DX11MeshID mMeshID;
        const MaterialID mDefaultMaterialID;
        const AABB mLocalAABB;
    };
}