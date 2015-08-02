#pragma once

#include "include/Resources/ModelNode.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"

namespace JonsEngine
{
    class Mesh
    {
    public:
        Mesh(const std::string name, const DX11MeshID meshID, const Vec3& minBounds, const Vec3& maxBounds);
        ~Mesh();

        //const ModelNodeID mParentNode;
        const std::string mName;
        const DX11MeshID mMeshID;
        const AABB mLocalAABB;

        DX11MaterialID mMaterialID;
        float mMaterialTilingFactor;
    };
}