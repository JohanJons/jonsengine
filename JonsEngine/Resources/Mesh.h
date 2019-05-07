#pragma once

#include "Resources/Material.h"
#include "Core/Types.h"
#include "Core/Math/AABB.h"

#include <string>

namespace JonsEngine
{
    class Mesh
    {
    public:
        Mesh(const std::string& name, const DX11MeshID meshID, const MaterialID defaultMaterialID, const Vec3& minBounds, const Vec3& maxBounds);
        ~Mesh();

        const std::string& GetName() const;
        DX11MeshID GetMesh() const;
        MaterialID GetDefaultMaterial() const;
        const AABB& GetLocalAABB() const;


    private:
         std::string mName;
         DX11MeshID mMeshID;
         MaterialID mDefaultMaterialID;
         AABB mLocalAABB;
    };
}